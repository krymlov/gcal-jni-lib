LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := gcal-11j

# -O2 is here for one reason: ndk-build defaults LOCAL_ARM_MODE to thumb, and its thumb release
# preset is "-mthumb -Oz -DNDEBUG" (build/core/toolchains/arm-linux-androideabi-clang/setup.mk),
# while arm64-v8a, x86 and x86_64 all get plain -O2. Confirmed present in the actual
# armeabi-v7a compile command (ndk-build V=1) before this line was added. So armeabi-v7a alone
# was being built in clang's *minimum size* mode - more aggressive about size than -Os - for a
# library that is almost entirely string/table lookups and date arithmetic. Nobody chose that;
# it fell out of the thumb default, same as in swe-android-lib. LOCAL_CFLAGS land after the
# NDK's own flags on the command line, so this -O2 wins for armeabi-v7a and is a no-op for the
# other three, which already had it (measured: byte-identical .so on arm64-v8a/x86/x86_64).
#
# Costs 10.1% on the armeabi-v7a .so (98 480 -> 108 472 bytes) and nothing on the others.
# Kept in thumb mode deliberately: -marm -O2 costs +36.2% there instead (134 152 bytes) for a
# codebase that is almost entirely small functions, where thumb-2's code density matters more
# than on swe's math-heavy library.
#
# Not raised to -O3: unlike the desktop build (where -O3 is the CMake Release default and
# fine), -O3 here grows every ABI dramatically - +82.5% armeabi-v7a, +77.1% arm64-v8a, +60.7%
# x86, +82.4% x86_64 (ai-github-projects/gcal-jni-lib/tools/opt-experiment-so.sh reproduces the
# table). This library is mostly small functions (string-table lookups, date getters), which
# -O3's more aggressive inlining multiplies out far worse than it did for swe's math-heavy one
# (+5-17% there). Not worth it on faith with no ARM device on this machine to measure any
# speed gain against that cost.
#
# Deliberately NOT copied over from the desktop build: -fno-math-errno. Verified byte-identical
# .text for all four ABIs with and without it (same finding as swe-android-lib) - Bionic's math
# functions do not set errno, so there is nothing for the flag to remove here either.
LOCAL_CFLAGS    += -O2

# -ffunction-sections/-fdata-sections are compiler code-gen flags; -Wl,-z,max-page-size=16384
# (required for Android 15+'s 16 KB page devices) and --gc-sections belong at the link stage.
# The first two used to be repeated in LOCAL_LDFLAGS as well, where the clang driver accepts
# and silently ignores them when only linking - no effect either way, same dead-weight shape
# swe-android-lib found in its own Android.mk.
LOCAL_LDFLAGS   += -Wl,-z,max-page-size=16384,--gc-sections

# -fvisibility=hidden is load-bearing here in a way it is not on Windows. These are ELF shared
# objects, so without it every non-static symbol would land in .dynsym; with it, only the
# JNIEXPORT one does. Result: exactly the one JNI entry point
# (Java_org_gaurabda_GCalManager_nativeGCal4Query) is exported, verified per ABI with
# llvm-nm --dynamic --defined-only --extern-only.
LOCAL_CFLAGS    += -ffunction-sections -fdata-sections -fvisibility=hidden -Wall
LOCAL_CPPFLAGS  += -ffunction-sections -fdata-sections -fvisibility=hidden -Wall
LOCAL_SRC_FILES := apc.cpp astro.cpp astro_arr.cpp astro_base.cpp astro_cr.cpp astro_moon.cpp astro_ms.cpp astro_res.cpp astro_sun.cpp astro_w.cpp avc.cpp gcal.cpp gc_fb.cpp gc_gstr.cpp gc_ui.cpp gmath.cpp tfile.cpp tstring.cpp ttimezone.cpp
include $(BUILD_SHARED_LIBRARY)