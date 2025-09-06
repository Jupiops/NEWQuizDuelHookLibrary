#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Split-APK aware extractor (no external deps) with versionCode matching
(including resourceID form) and versioned global-metadata output.

Extracts:
  • versionName + versionCode from AndroidManifest.xml (AXML → XML via built-in decompressor)
  • assets/**/global-metadata.dat → global-metadata-<versionName>.dat (first found)
  • lib/<abi>/libil2cpp.so → libil2cpp-<versionName>-<abi>.so

Inputs supported:
  - A single APK
  - Multiple APKs (e.g., base.apk split_config.*.apk …)
  - A directory containing APKs
  - An .apks/.xapk/.zip archive (it will scan all inner *.apk files)

Usage examples:
  python extract_unity_bits.py base.apk split_config.arm64_v8a.apk
  python extract_unity_bits.py /path/to/folder_with_apks
  python extract_unity_bits.py bundle.apks --version-code "resourceID 0x6db117d"
  python extract_unity_bits.py app.apk -o se.maginteractive.quizduel2 --overwrite
"""
import argparse
import io
import re
import shutil
import sys
import zipfile
from pathlib import Path
from typing import Iterable, List, Tuple, Optional, Dict, Any

OUTPUT_DIR_NAME = "se.maginteractive.quizduel2"
VERSION_NAME_FALLBACK = "unknown"


# ----------------- AXML → XML Decompressor (no external deps) -----------------
class AndroidXMLDecompress:
    endDocTag = 0x00100101
    startTag = 0x00100102
    endTag = 0x00100103

    def decompressXML(self, xml: bytes) -> str:
        xml = bytearray(xml)
        finalXml = ""

        numStrings = self.LEW(xml, 4 * 4)
        sitOff = 0x24
        stOff = sitOff + numStrings * 4

        xmlTagOff = self.LEW(xml, 3 * 4)
        for i in range(xmlTagOff, len(xml) - 4, 4):
            if self.LEW(xml, i) == self.startTag:
                xmlTagOff = i
                break

        off = xmlTagOff
        while off < len(xml):
            tag0 = self.LEW(xml, off)
            nameSi = self.LEW(xml, off + 5 * 4)

            if tag0 == self.startTag:
                numAttrs = self.LEW(xml, off + 7 * 4)
                off += 9 * 4
                name = self.compXmlString(xml, sitOff, stOff, nameSi) or "node"
                attrs = ""
                for _ in range(numAttrs):
                    attrNameSi = self.LEW(xml, off + 1 * 4)
                    attrValueSi = self.LEW(xml, off + 2 * 4)
                    attrResId = self.LEW(xml, off + 4 * 4)
                    off += 5 * 4

                    attrName = self.compXmlString(xml, sitOff, stOff, attrNameSi) or "attr"
                    if attrValueSi != -1:
                        attrValue = self.compXmlString(xml, sitOff, stOff, attrValueSi) or ""
                    else:
                        attrValue = "resourceID " + hex(attrResId)
                    attrs += f' {attrName}="{attrValue}"'
                finalXml += f"<{name}{attrs}>"
            elif tag0 == self.endTag:
                off += 6 * 4
                name = self.compXmlString(xml, sitOff, stOff, nameSi) or "node"
                finalXml += f"</{name}>"
            elif tag0 == self.endDocTag:
                break
            else:
                break
        return finalXml

    def compXmlString(self, xml: bytearray, sitOff: int, stOff: int, strInd: int) -> Optional[str]:
        if strInd < 0:
            return None
        strOff = stOff + self.LEW(xml, sitOff + strInd * 4)
        return self.compXmlStringAt(xml, strOff)

    def compXmlStringAt(self, arr: bytearray, strOff: int) -> str:
        strlen = ((arr[strOff + 1] << 8) & 0xFF00) | (arr[strOff] & 0xFF)
        chars = bytearray()
        for i in range(strlen):
            chars.append(arr[strOff + 2 + i * 2])
        return chars.decode("utf-8", errors="replace")

    def LEW(self, arr: bytearray, off: int) -> int:
        c = ((arr[off + 3] << 24) & 0xFF000000) | ((arr[off + 2] << 16) & 0x00FF0000) | \
            ((arr[off + 1] << 8) & 0x0000FF00) | (arr[off] & 0x000000FF)
        if c & 0x80000000:
            c = -((~c & 0xFFFFFFFF) + 1)
        return c


# ------------------------------- helpers --------------------------------------
def die(msg: str, code: int = 1):
    print(f"[!] {msg}", file=sys.stderr)
    sys.exit(code)


def is_archive_of_apks(path: Path) -> bool:
    return path.suffix.lower() in {".apks", ".xapk", ".zip"}


def gather_input_apks(paths: List[Path]) -> List[Tuple[str, io.BytesIO]]:
    """
    Normalize all inputs into a list of (label, file_like) pairs containing APK bytes.
    label is just for messages (e.g., filename or archive member path).
    """
    result: List[Tuple[str, io.BytesIO]] = []

    def add_apk_bytes(label: str, data: bytes):
        result.append((label, io.BytesIO(data)))

    for p in paths:
        if p.is_dir():
            for sub in sorted(p.rglob("*.apk")):
                add_apk_bytes(str(sub), sub.read_bytes())
        elif p.is_file():
            if p.suffix.lower() == ".apk":
                add_apk_bytes(str(p), p.read_bytes())
            elif is_archive_of_apks(p):
                try:
                    with zipfile.ZipFile(p, "r") as z:
                        for info in z.infolist():
                            if info.filename.lower().endswith(".apk"):
                                add_apk_bytes(f"{p}!{info.filename}", z.read(info))
                except zipfile.BadZipFile:
                    die(f"Invalid archive: {p}")
            else:
                die(f"Unsupported file type: {p}")
        else:
            die(f"Path not found: {p}")
    if not result:
        die("No APKs found in the given inputs.")
    return result


def zipfiles_from_bytesio(label: str, bio: io.BytesIO) -> zipfile.ZipFile:
    try:
        return zipfile.ZipFile(bio, "r")
    except zipfile.BadZipFile:
        die(f"The file is not a valid APK/ZIP: {label}")


def read_manifest_from_zip(zf: zipfile.ZipFile) -> Optional[bytes]:
    try:
        return zf.read("AndroidManifest.xml")
    except KeyError:
        return None


def parse_manifest(axml_bytes: bytes) -> Dict[str, Any]:
    """
    Returns dict with:
      - 'versionName' (str or 'unknown')
      - 'versionCode' (str or None), e.g. "123456" OR "resourceID 0x6db117d"
    """
    dec = AndroidXMLDecompress()
    out = {"versionName": VERSION_NAME_FALLBACK, "versionCode": None}
    try:
        xml_text = dec.decompressXML(axml_bytes)
    except Exception as e:
        print(f"[!] AXML decompress failed: {e}", file=sys.stderr)
        return out

    # versionName
    m = re.search(r'\bandroid:versionName="([^"]+)"', xml_text)
    if not m:
        m = re.search(r'\bversionName="([^"]+)"', xml_text)
    if m:
        out["versionName"] = m.group(1)

    # versionCode (string: either digits or "resourceID 0x..."). Try android: first, then plain.
    m = re.search(r'\bandroid:versionCode="([^"]+)"', xml_text)
    if not m:
        m = re.search(r'\bversionCode="([^"]+)"', xml_text)
    if m:
        out["versionCode"] = m.group(1).strip()

    return out


def choose_base_first(apk_labels: Iterable[str]) -> List[str]:
    """
    Sort helper: base.* first, then normal, then split_config.* last.
    """

    def key(lbl: str):
        low = lbl.lower()
        if "/base.apk" in low or low.endswith("base.apk") or "base-" in low:
            return (0, low)
        if "split_config" in low or "config." in low:
            return (2, low)
        return (1, low)

    return sorted(apk_labels, key=key)


def find_entries_with_suffix(zf: zipfile.ZipFile, suffix: str):
    for info in zf.infolist():
        if info.filename.endswith(suffix):
            yield info


# --------------------------------- main ---------------------------------------
def main():
    ap = argparse.ArgumentParser(description="Split-APK aware extractor for Unity IL2CPP bits (versionCode-matched).")
    ap.add_argument("inputs", nargs="+", type=Path, help="APK(s), directory, or .apks/.xapk/.zip")
    ap.add_argument("-o", "--outdir", type=Path, default=Path(OUTPUT_DIR_NAME),
                    help=f"Output directory (default: {OUTPUT_DIR_NAME})")
    ap.add_argument("--overwrite", action="store_true", help="Overwrite existing files")
    ap.add_argument("--version-code", type=str, default=None,
                    help='Only use APKs whose manifest versionCode equals this exact string '
                         '(e.g., "123456" or "resourceID 0x6db117d")')
    args = ap.parse_args()

    outdir: Path = args.outdir
    outdir.mkdir(parents=True, exist_ok=True)

    # Load all APK bytes
    apk_items = gather_input_apks(args.inputs)  # List[(label, BytesIO)]

    # Parse manifest info for each APK
    infos: List[Dict[str, Any]] = []
    for label, bio in apk_items:
        zf = zipfiles_from_bytesio(label, bio)
        m = read_manifest_from_zip(zf)
        vName = VERSION_NAME_FALLBACK
        vCodeRaw = None
        if m:
            parsed = parse_manifest(m)
            vName = parsed["versionName"]
            vCodeRaw = parsed["versionCode"]
        infos.append({"label": label, "bio": bio, "versionName": vName, "versionCode": vCodeRaw})

    # Decide which versionCode to use
    target_vc: Optional[str] = args.version_code
    if target_vc is None:
        # Prefer versionCode from anything named like base.apk
        ordered_labels = choose_base_first([i["label"] for i in infos])
        for lbl in ordered_labels:
            item = next(i for i in infos if i["label"] == lbl)
            if item["versionCode"]:
                target_vc = item["versionCode"]
                break
        # If still None, pick the most common versionCode among inputs (ignores None)
        if target_vc is None:
            counts: Dict[str, int] = {}
            for i in infos:
                if i["versionCode"]:
                    counts[i["versionCode"]] = counts.get(i["versionCode"], 0) + 1
            if counts:
                target_vc = max(counts.items(), key=lambda kv: kv[1])[0]

    if target_vc is None:
        print("[!] Could not determine a versionCode. Proceeding with ALL APKs (may mix splits).")
        selected = infos
    else:
        selected = [i for i in infos if i["versionCode"] == target_vc]
        if not selected:
            die(f'No APKs matched --version-code "{args.version_code}".')
        print(f'[*] Using versionCode = "{target_vc}" across {len(selected)} APK(s).')

    # Determine versionName from selected (prefer a base.*)
    version_name = VERSION_NAME_FALLBACK
    for lbl in choose_base_first([i["label"] for i in selected]):
        item = next(i for i in selected if i["label"] == lbl)
        if item["versionName"] != VERSION_NAME_FALLBACK:
            version_name = item["versionName"]
            print(f"[*] versionName ({item['label']}): {version_name}")
            break
    if version_name == VERSION_NAME_FALLBACK:
        for item in selected:
            if item["versionName"] != VERSION_NAME_FALLBACK:
                version_name = item["versionName"]
                print(f"[*] versionName ({item['label']}): {version_name}")
                break
    if version_name == VERSION_NAME_FALLBACK:
        print("[!] Could not determine versionName. Filenames will use 'unknown'.")

    # ---- Extract global-metadata.dat (first found in selected) → versioned name ----
    metadata_extracted = False
    for item in selected:
        label, bio = item["label"], item["bio"]
        zf = zipfiles_from_bytesio(label, bio)
        candidates = list(find_entries_with_suffix(zf, "global-metadata.dat"))
        if candidates:
            chosen = candidates[0]
            target = outdir / f"global-metadata-{version_name}.dat"
            if target.exists() and not args.overwrite:
                print(f"[=] Skipping {target.name} (exists). Use --overwrite to replace.")
            else:
                tmp = outdir / (target.name + ".tmp")
                with zf.open(chosen) as src, tmp.open("wb") as dst:
                    shutil.copyfileobj(src, dst)
                tmp.replace(target)
                print(f"[+] Extracted: {label}:{chosen.filename} -> {target}")
            if len(candidates) > 1:
                print("[i] Multiple global-metadata.dat found in this APK:")
                for c in candidates:
                    print(f"    - {label}:{c.filename}")
            metadata_extracted = True
            break
    if not metadata_extracted:
        print("[!] No global-metadata.dat found in selected APKs.")

    # ---- Extract lib/<abi>/libil2cpp.so from selected → versioned names ----
    found_any_lib = False
    for item in selected:
        label, bio = item["label"], item["bio"]
        zf = zipfiles_from_bytesio(label, bio)
        for info in zf.infolist():
            if info.filename.startswith("lib/") and info.filename.endswith("/libil2cpp.so") and info.filename.count(
                    "/") >= 2:
                parts = info.filename.split("/")
                abi = parts[1] if len(parts) >= 3 else "unknown-abi"
                out_name = f"libil2cpp-{version_name}-{abi}.so"
                target = outdir / out_name
                if target.exists() and not args.overwrite:
                    print(f"[=] Skipping {out_name} (exists). Use --overwrite to replace.")
                    continue
                tmp = outdir / (out_name + ".tmp")
                with zf.open(info) as src, tmp.open("wb") as dst:
                    shutil.copyfileobj(src, dst)
                tmp.replace(target)
                print(f"[+] Extracted: {label}:{info.filename} -> {target}")
                found_any_lib = True
    if not found_any_lib:
        print("[!] No lib/<abi>/libil2cpp.so found in selected APKs.")

    print(f"[*] Done. Output directory: {outdir.resolve()}")


if __name__ == "__main__":
    main()
