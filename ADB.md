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
