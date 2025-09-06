#!/usr/bin/env python3
import argparse
import json
import logging
import mmap
import pathlib
import re
from collections import defaultdict
from typing import DefaultDict, Dict, Iterable, List, Tuple

from elftools.elf.elffile import ELFFile

# Methods with names and signatures to look for in the scripts
methods: List[Tuple[str, str]] = [
    ('System.String$$Concat',
     'System_String_o* System_String__Concat (System_String_o* str0, System_String_o* str1, const MethodInfo* method);'),
    ('System.String$$CreateString',
     'System_String_o* System_String__CreateString (System_String_o* __this, int8_t* value, int32_t startIndex, int32_t length, const MethodInfo* method);'),
    ('VIPManager$$HasVIPProperty', 'bool VIPManager__HasVIPProperty (int32_t property, const MethodInfo* method);'),
    ('QuizDuel.Classic.QuestionContainerClassic$$GetTimerDuration',
     'float QuizDuel_Classic_QuestionContainerClassic__GetTimerDuration (QuizDuel_Classic_QuestionContainerClassic_o* __this, const MethodInfo* method);'),
    ('QuizDuel.QuestionAnswerButton$$Init',
     'void QuizDuel_QuestionAnswerButton__Init (QuizDuel_QuestionAnswerButton_o* __this, int32_t answerIndex, System_String_o* text, MAG_UI_ButtonClick_o* onClick, const MethodInfo* method);'),
]


def default_root() -> pathlib.Path:
    """Return the default root folder where versioned `script-*.json` and `libil2cpp-*.so` live."""
    return pathlib.Path(__file__).parent.absolute().joinpath('se.maginteractive.quizduel2')


def load_files(root: pathlib.Path) -> List[pathlib.Path]:
    """List files under the given root directory."""
    if not root.exists() or not root.is_dir():
        raise FileNotFoundError(f"Root directory not found: {root}")
    return [f for f in root.iterdir() if f.is_file()]


def organize_files(files: Iterable[pathlib.Path]) -> Dict[str, Dict[str, Dict[str, pathlib.Path]]]:
    """
    Group files by version and architecture.
    Expects names like: script-<ver>-<arch>.json and libil2cpp-<ver>-<arch>.so
    Returns: organized[version][arch]['script'|'libil2cpp'] -> Path
    """
    # Initialize a nested dictionary to store files by version, architecture and type
    organized: DefaultDict[str, DefaultDict[str, Dict[str, pathlib.Path]]] = defaultdict(lambda: defaultdict(dict))

    # Regex pattern to extract version and architecture from filenames
    pattern = re.compile(r'(libil2cpp|script)-([\d.]+)-(arm64-v8a|armeabi-v7a)\.(so|json)$')

    for file in files:
        m = pattern.match(file.name)
        if not m:
            continue

        # Extract details from the filename
        file_type, version, arch, _ext = m.groups()

        # Organize the file into the nested dictionary
        organized[version][arch][file_type] = file

    return organized


def get_text_segment_base(libil2cpp_path: pathlib.Path) -> Tuple[int, int]:
    """
    Extract the base virtual address and file offset of the executable PT_LOAD segment from the ELF.
    These are used to map method virtual addresses into file offsets.
    """
    with open(libil2cpp_path, "rb") as f:
        elf = ELFFile(f)
        for segment in elf.iter_segments():
            if segment['p_type'] == 'PT_LOAD' and (segment['p_flags'] & 0x1):
                return int(segment['p_vaddr']), int(segment['p_offset'])
    return 0, 0


def collect_raw_patterns(script_data: dict,
                         lib_file_path: pathlib.Path,
                         base_vaddr: int,
                         base_offset: int,
                         pattern_len: int) -> Tuple[Dict[str, List[bytes]], Dict[str, List[int]]]:
    """
    Collect raw code bytes for each desired method.
    Reads `pattern_len` bytes from the file at the mapped offset for the method address.
    Returns:
      - raw[name] -> list of byte strings (one per version/arch present)
      - offs[name] -> list of file offsets corresponding to the reads
    """
    raw: DefaultDict[str, List[bytes]] = defaultdict(list)
    offs: DefaultDict[str, List[int]] = defaultdict(list)

    script_methods = script_data.get('ScriptMethod')
    if not isinstance(script_methods, list):
        logging.warning("Script JSON missing `ScriptMethod` array")
        return raw, offs

    with open(lib_file_path, "rb") as lib_file, mmap.mmap(lib_file.fileno(), 0, access=mmap.ACCESS_READ) as mm:
        size = mm.size()
        wanted = {(n, s) for (n, s) in methods}
        for entry in script_methods:
            name = entry.get('Name')
            sig = entry.get('Signature')
            if (name, sig) not in wanted:
                continue

            # Adjust the address based on the base address
            method_address = int(entry.get('Address', 0))
            file_offset = method_address - base_vaddr + base_offset

            # Ensure the offset is valid
            if file_offset < 0:
                logging.warning("Negative offset for %s at 0x%x in %s", name, method_address, lib_file_path)
                continue
            if file_offset + pattern_len > size:
                logging.warning("Out-of-bounds read for %s at file offset 0x%x in %s", name, file_offset,
                                lib_file_path)
                continue

            # Read from the binary file at the adjusted file offset
            mm.seek(file_offset)
            data = mm.read(pattern_len)
            if len(data) != pattern_len:
                logging.warning("Short read for %s at file offset 0x%x", name, file_offset)
                continue

            # Store the binary data as a raw pattern
            raw[name].append(data)
            offs[name].append(file_offset)

    return raw, offs


def signature_from_patterns(patterns: List[bytes]) -> str:
    """
    Build a space-separated signature string of hex bytes, using '?' where bytes differ across patterns.
    All `patterns` must have the same length.
    """
    if not patterns:
        return ""
    length = len(patterns[0])
    if any(len(p) != length for p in patterns):
        raise ValueError("Inconsistent pattern lengths while generating signature")
    parts: List[str] = []
    for i in range(length):
        col = [p[i] for p in patterns]
        b0 = col[0]
        if all(b == b0 for b in col):
            parts.append(f"{b0:02X}")
        else:
            parts.append("?")
    return " ".join(parts)


def signature_to_regex_bytes(signature: str) -> bytes:
    """
    Convert a signature like 'AA BB ? CC' into an ASCII bytes regex: rb'\\xAA\\xBB.\\xCC'
    """
    out: List[str] = []
    for tok in signature.split():
        if tok in ("?", "??"):
            out.append(".")
        else:
            out.append(f"\\x{tok}")
    return "".join(out).encode("ascii")


def find_signature_offsets(file_path: pathlib.Path, signature: str) -> List[int]:
    """
    Search for `signature` in `file_path` and return a list of file offsets for all matches.
    """
    if not signature:
        return []
    regex = re.compile(signature_to_regex_bytes(signature), re.DOTALL)
    offsets: List[int] = []
    with open(file_path, "rb") as f, mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ) as mm:
        for m in regex.finditer(mm):
            offsets.append(m.start())
    return offsets


def validate_methods_list() -> bool:
    """
    Validate the `methods` global is a flat list of (str, str) tuples.
    Returns False and logs errors if malformed.
    """
    if not isinstance(methods, list):
        logging.error("`methods` must be a list of (name, signature) tuples.")
        return False
    ok = True
    for i, item in enumerate(methods):
        if not (isinstance(item, tuple) and len(item) == 2 and all(isinstance(x, str) for x in item)):
            logging.error("Invalid entry at methods[%d]: expected (str, str) tuple, got %r", i, item)
            ok = False
    return ok


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate cross-version signature patterns from il2cpp dumps.")
    parser.add_argument("--root", type=pathlib.Path, default=default_root(),
                        help="Root directory containing versioned files")
    # Starting pattern length. In dynamic mode this is the initial length to try.
    parser.add_argument("--pattern-length", type=int, default=32,
                        help="Bytes to sample per method (or starting length in dynamic mode)")
    # Dynamic mode options.
    parser.add_argument("--dynamic-pattern", action="store_true",
                        help="Grow pattern length until the first match equals the known method offset")
    parser.add_argument("--max-pattern-length", type=int, default=128,
                        help="Upper bound for dynamic pattern length search")
    parser.add_argument("--pattern-step", type=int, default=8,
                        help="Increment when growing pattern length in dynamic mode")
    parser.add_argument("--verbose", action="store_true", help="Enable debug logging")
    args = parser.parse_args("--dynamic-pattern".split())

    logging.basicConfig(level=logging.DEBUG if args.verbose else logging.INFO, format="%(levelname)s: %(message)s")

    if not methods:
        logging.error("`methods` list is empty. Populate it with (name, signature) pairs.")
        return
    if not validate_methods_list():
        return

    files = load_files(args.root)
    organized = organize_files(files)

    per_method_patterns: Dict[str, List[bytes]] = defaultdict(list)  # name -> [bytes]
    per_method_actual_offs: Dict[str, Dict[str, List[int]]] = defaultdict(
        lambda: defaultdict(list))  # name -> "ver:arch" -> [offs]

    # Collect byte windows. If dynamic, read the max length once so we can try smaller prefixes.
    collect_len = max(args.pattern_length, args.max_pattern_length) if args.dynamic_pattern else args.pattern_length

    for version, arch_map in organized.items():
        for arch, group in arch_map.items():
            script_path = group.get("script")
            lib_path = group.get("libil2cpp")
            if not script_path or not lib_path:
                logging.warning("Missing script/lib for version=%s arch=%s", version, arch)
                continue

            try:
                script_data = json.loads(script_path.read_text(encoding="utf-8"))
            except Exception as e:
                logging.error("Failed to read %s: %s", script_path, e)
                continue

            base_vaddr, base_off = get_text_segment_base(lib_path)
            logging.info("Version=%s Arch=%s BaseVA=%#x BaseOff=%#x VirtOff=%#x",
                         version, arch, base_vaddr, base_off, (base_vaddr - base_off))

            raw, actual = collect_raw_patterns(script_data, lib_path, base_vaddr, base_off, collect_len)

            for name, patt_list in raw.items():
                per_method_patterns[name].extend(patt_list)
                per_method_actual_offs[name][f"{version}:{arch}"].extend(actual.get(name, []))

    # Utility to build a signature at an exact length from larger collected windows.
    def build_sig_at_length(patterns: List[bytes], length: int) -> str:
        return signature_from_patterns([p[:length] for p in patterns])

    # In dynamic mode, grow the signature length until the first hit equals the known offset
    # for every version/arch that has an actual offset. Otherwise, use fixed length.
    signatures: Dict[str, str] = {}
    chosen_lengths: Dict[str, int] = {}

    for name, patterns in per_method_patterns.items():
        if not patterns:
            continue

        if not args.dynamic_pattern:
            L = args.pattern_length
            sig = build_sig_at_length(patterns, L)
            signatures[name] = sig
            chosen_lengths[name] = L
            continue

        # Dynamic search.
        start = max(1, args.pattern_length)
        upper = max(start, args.max_pattern_length)
        step = max(1, args.pattern_step)

        # Limit by the actually collected window size.
        max_available = min(len(p) for p in patterns)
        upper = min(upper, max_available)

        selected_sig = ""
        selected_len = upper  # fallback to the largest tried

        # Try increasing lengths until the first match equals the known offset everywhere.
        L = start
        while L <= upper:
            sig = build_sig_at_length(patterns, L)
            ok_everywhere = True

            for version, arch_map in organized.items():
                for arch, group in arch_map.items():
                    if "libil2cpp" not in group:
                        continue
                    actual_list = per_method_actual_offs[name].get(f"{version}:{arch}", [])
                    if not actual_list:
                        # If we lack ground truth for this version/arch, skip it.
                        continue

                    lib_path = group["libil2cpp"]
                    found = find_signature_offsets(lib_path, sig)

                    # Require a non-empty match and that the first hit equals the actual offset.
                    if not found or found[0] != actual_list[0]:
                        ok_everywhere = False
                        break
                if not ok_everywhere:
                    break

            if ok_everywhere:
                selected_sig = sig
                selected_len = L
                break

            L += step

            # Avoid building a signature larger than the available data.
            if L > max_available:
                break

        if not selected_sig:
            # Fallback to the longest tried prefix if none satisfied the condition.
            selected_sig = build_sig_at_length(patterns, selected_len)

        signatures[name] = selected_sig
        chosen_lengths[name] = selected_len

    # Report results and a quick verification snapshot.
    print("\nFinal Signature Patterns Across Versions:")
    for name, sig in signatures.items():
        print(f"Method: {name}")
        print(f"  Signature Length: {chosen_lengths.get(name, len(sig.split()))} bytes")
        print(f"  Signature Pattern: {sig}")
        for version, arch_map in organized.items():
            for arch, group in arch_map.items():
                if "libil2cpp" not in group:
                    continue
                lib_path = group["libil2cpp"]
                found = [hex(o) for o in find_signature_offsets(lib_path, sig)][:7]
                actual_list = per_method_actual_offs[name].get(f"{version}:{arch}", [])
                actual_hex = hex(actual_list[0]) if actual_list else "N/A"
                match = "Yes" if found and actual_list and found[0] == actual_hex else "No"
                print(
                    f"  Version: {version}, Architecture: {arch}, Actual Offset: {actual_hex}, Offsets: {found}, Match: {match}")


if __name__ == '__main__':
    main()
