# All four ABIs Google Play still accepts. armeabi-v7a and x86 are 32-bit and increasingly
# rare, but dropping either is a distribution decision, not a build one.
#
# These do not compute bit-identical results to each other, and cannot be made to without a
# flag change: arm64-v8a has FMA in its base ISA and clang contracts a*b+c into a single fmadd
# by default (147 such instructions in this library's object files, confirmed by
# opt-experiment-so.sh's fingerprint; none on the other three ABIs). Unlike swe-android-lib,
# there is no per-ABI cross-check test suite here yet to say how far that moves any printed
# date/time - worth building one before relying on this ABI's output matching the desktop DLL
# to the same precision the other three do. -ffp-contract=off would close the gap at a cost in
# speed; not applied without a way to measure whether it changes anything observable first.
APP_ABI := armeabi-v7a arm64-v8a x86_64 x86

# minSdk. Bumping this is an app-compatibility decision, not a build tuning one.
APP_PLATFORM := android-21

# release: -DNDEBUG and the per-ABI optimisation preset. Android.mk overrides the -O level for
# armeabi-v7a; see the comment there for why.
APP_OPTIM := release
