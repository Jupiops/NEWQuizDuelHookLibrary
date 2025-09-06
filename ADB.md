# Useful ADB commands

## Retrieve the logs from the device

```shell
adb -d logcat HOOK_LIBRARY:D *:S
```

## List the installed packages

```shell
adb shell pm list packages
```

```shell
adb shell pm list packages -f -3
```

```shell
adb shell pm path se.maginteractive.quizduel2
```

## Retrieve the APK from the device

```shell
adb pull /data/app/se.maginteractive.quizduel2-1/base.apk
```

> Note: When getting a 'Permission denied' error, you may need to root your device or use `adb backup` instead. Another
> workaround is to copy the APK to a readable location first:

```shell
adb shell ls /sdcard/
adb shell cp /data/app/se.maginteractive.quizduel2-1/base.apk /sdcard/
adb pull /sdcard/base.apk
```

## Merges multiple splitted apk files (directory, xapk, apkm, apks ...) to standalone apk

```shell
java -jar APKEditor-*.jar  m -i /path/to/apk/files
```
