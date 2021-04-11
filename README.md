# NEW QuizDuel! Hook library

based on the [template](https://github.com/Djngo/Hooking-and-Patching-android-template)
from [Djangø](https://github.com/Djngo)
---

**Disclaimer: For educational purposes only**

Hooks into the [NEW QuizDuel!](https://play.google.com/store/apps/details?id=se.maginteractive.quizduel2) android
application and applies some nice features

## Loading the libhook.so

First we need to compile our project.

* Edit params.txt & paste your ndk-build **path location** on the very first line and the **project location** on the
  second line like I did.
* Start Build.bat and follow instructions.
* Once compiled you'll have 2 new file in x32 and x64 named **libs** and **obj**. In order to load your <b>
  libhook.so</b>, you'll need to decompile the apk, you can use Apk Easy Tool for this.
* Go to *libs* and copy the created libhook.so in the *lib* folder of your game.
* Navigate to "<b>/smali_classes3</b>" & copy the loadLib from this git inside of that folder.
* Open the AndroidManifest.xml file in the decompiled apk and search this

```xml
android:name="android.intent.action.MAIN"
```

it will give us the path of the MainActivity so in our case it's at
"<b>com.google.firebase.MessagingUnityPlayerActivity</b>" but using "<b>com.unity3d.player.UnityPlayerActivity</b>"
as it's stated in most other tutorials is working fine too.

* Open the file you've chosen with Notepad++ or any other preferred text editing software and search something like
  this:

```smali
.method protected onCreate(Landroid/os/Bundle;)V
    .locals 2
```

* Once found, paste this code under .locals 2:

```smali
invoke-static {}, Lcom/loadLib/libLoader;->loadLib()V
```

* Step 1: Recompile the apk
* Step 2: Profit
  <br>

## Useful links

* [NdkBuild | Android Developers](https://developer.android.com/reference/tools/gradle-api/4.1/com/android/build/api/dsl/NdkBuild)
* [Modding a Unity C++ Android Game](https://www.areizen.fr/post/modding-unity-game/)
* [How to Mod an Android Game](https://vipmods.net/how-to-mod-an-android-game/)
* [List of modding related Github links](https://www.andnixsh.com/2020/06/list-of-modding-related-github-links.html)
* [Android Modding Tutorial Part 1](https://guidedhacking.com/threads/android-modding-tutorial-part-1.13124/)
* [ARM To HEX Converter Online](https://armconverter.com/)
* [Project Winter Multi-Hack](https://github.com/EquiFox/ProjectWinterHack)