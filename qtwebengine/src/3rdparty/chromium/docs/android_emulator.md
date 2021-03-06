# Using an Android Emulator
Always use x86 emulators (or x86\_64 for testing 64-bit APKs). Although arm
emulators exist, they are so slow that they are not worth your time.

[TOC]

## Building for Emulation
You need to target the correct architecture via GN args:
```gn
target_cpu = "x86"  # or "x64" if you have an x86_64 emulator
```

## Creating an Emulator Image
By far the easiest way to set up emulator images is to use Android Studio.
If you don't have an [Android Studio project](android_studio.md) already, you
can create a blank one to be able to reach the Virtual Device Manager screen.

Refer to: https://developer.android.com/studio/run/managing-avds.html

Where files live:
 * System partition images are stored within the sdk directory.
 * Emulator configs and data partition images are stored within
   `~/.android/avd/`.

### Choosing a Skin
Choose a skin with a small screen for better performance (unless you care about
testing large screens).

### Choosing an Image
Android Studio's image labels roughly translate to the following:

| AVD "Target" | Virtual Device Configuration tab | GMS? | Build Properties |
| --- | --- | --- | --- |
| Google Play | "Recommended" (the default tab) | This has GMS | `user`/`release-keys` |
| Google APIs | "x86 Images" | This has GMS | `userdebug`/`dev-keys` |
| No label | "x86 Images" | AOSP image, does not have GMS | `eng`/`test-keys` |

*** promo
**Tip:** if you're not sure which to use, choose **Google APIs** under the **x86
Images** tab in the Virtual Device Configuration wizard.
***

### Configuration
"Show Advanced Settings" > scroll down:
* Set internal storage to 4000MB (component builds are really big).
* Set SD card to 1000MB (our tests push a lot of files to /sdcard).

### Known Issues
 * Our test & installer scripts do not work with pre-MR1 Jelly Bean.
 * Component builds do not work on pre-KitKat (due to the OS having a max
   number of shared libraries).
 * Jelly Bean and KitKat images sometimes forget to mount /sdcard :(.
   * This causes tests to fail.
   * To ensure it's there: `adb -s emulator-5554 shell mount` (look for /sdcard)
   * Can often be fixed by editing `~/.android/avd/YOUR_DEVICE/config.ini`.
     * Look for `hw.sdCard=no` and set it to `yes`

## Starting an Emulator from the Command Line
Refer to: https://developer.android.com/studio/run/emulator-commandline.html.

*** promo
Ctrl-C will gracefully close an emulator.
***

*** promo
**Tip:** zsh users can add https://github.com/zsh-users/zsh-completions to
provide tab completion for the `emulator` command line tool.
***

### Basic Command Line Use
```shell
$ # List virtual devices that you've created:
$ ~/Android/Sdk/emulator/emulator -list-avds
$ # Start a named device:
$ ~/Android/Sdk/emulator/emulator @EMULATOR_ID
```

### Running a Headless Emulator
You can run an emulator without creating a window on your desktop (useful for
`ssh`):
```shell
$ ~/Android/Sdk/emulator/emulator -no-window @EMULATOR_ID
$ # This also works for new enough emulator builds:
$ ~/Android/Sdk/emulator/emulator-headless @EMULATOR_ID
```

### Running Multiple Emulators
Tests are automatically sharded amongst available devices. If you run multiple
emulators, then running test suites becomes much faster. Refer to the
"Multiple AVD instances" section of these [emulator release notes](
https://androidstudio.googleblog.com/2018/11/emulator-28016-stable.html)
for more about how this works.
```shell
$ # Start 8 emulators. Press Ctrl-C to stop them all.
$ ( for i in $(seq 8); do ~/Android/Sdk/emulator/emulator @EMULATOR_ID -read-only & done; wait )
$ # Start 12 emulators. More than 10 requires disabling audio on some OS's. Reducing cores increases parallelism.
$ ( for i in $(seq 12); do ~/Android/Sdk/emulator/emulator @EMULATOR_ID -read-only -no-audio -cores 2 & done; wait )
```

### Writable system partition
Unlike physical devices, an emulator's `/system` partition cannot be modified by
default (even on rooted devices). If you need to do so (such as to remove a
system app), you can start your emulator like so:
```shell
$ ~/Android/Sdk/emulator/emulator -writable-system @EMULATOR_ID
```

## Using an Emulator
 * Emulators show up just like devices via `adb devices`
   * Device serials will look like "emulator-5554", "emulator-5556", etc.

## Emulator pros and cons

### Pros
 * **Compiles are faster.** Many physical devices are arm64, whereas emulators
   are typically x86 (32-bit). 64-bit builds may require 2 copies of the native
   library (32-bit and 64-bit), so compiling for an arm64 phone is ~twice as
   much work as for an emulator (for targets which support WebView).
 * **APKs install faster.** Since emulators run on your workstation, adb can
   push the APK onto the emulator without being [bandwidth-constrained by
   USB](https://youtu.be/Mzop8bXZI3E).
 * Emulators can be nice for working remotely. Physical devices usually require
   `scp` or ssh port forwarding to copy the APK from your workstation and
   install on a local device. Emulators run on your workstation, so there's **no
   ssh slow-down**.

### Cons
 * If you're investigating a hardware-specific bug report, you'll need a
   physical device with the actual hardware to repro that issue.
 * x86 emulators need a separate out directory, so building for both physical
   devices and emulators takes up more disk space (not a problem if you build
   exclusively for the emulator).
 * `userdebug`/`eng` emulators don't come with the Play Store installed, so you
   can't install third party applications. Sideloading is tricky, as not all
   third-party apps support x86.
