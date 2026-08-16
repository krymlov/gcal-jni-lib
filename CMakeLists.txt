cmake_minimum_required(VERSION 3.17)

project(gcal-11j)

set(CMAKE_CXX_STANDARD 11)

# -static: 'swed'-style TLS is not used here (gcal has no per-thread globals), but without a
# fully static link System.loadLibrary() fails unless a MinGW bin directory happens to be on
# PATH (see swe-jni-lib/CMakeLists.txt for the general shape of that problem). CMake passes
# CXX flags to the link line too, so putting -static here rather than on
# CMAKE_SHARED_LINKER_FLAGS still produces a fully static link - build-dll.sh verifies the
# result imports only KERNEL32.dll and msvcrt.dll and loads with an empty PATH.
#
# -fno-math-errno lets GCC emit inline math instructions (sqrt and friends) instead of calling
# into libm and then branching to set errno; nothing in this library or above it reads errno.
# Measured on this toolchain (ai-github-projects/gcal-jni-lib/tools/opt-experiment.sh
# reproduces the table below): a small but reproducible ~0.4% faster, no size change, bit-
# identical output. Much smaller than the 3.5% swe-jni-lib measured for the same flag on the
# same toolchain - that workload is dominated by raw libm calls across a full ephemeris chart,
# this one by GetNextConjunction/GetPrevConjunction's day-stepping search, which has less
# errno-branch overhead to remove. Kept anyway: it is free (zero size cost, fingerprint
# identical to the baseline) and never measured slower.
#
# All sources below are .cpp, so - unlike swe-jni-lib before its CMAKE_C_FLAGS/CMAKE_CXX_FLAGS
# fix - these flags reach the compiler for every translation unit without needing a language
# check.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -Wall -fno-math-errno")

# Release only: strip the symbol table from the shipped DLL. The one exported entry point
# (Java_org_gaurabda_GCalManager_nativeGCal4Query) lives in .edata and is untouched by this -
# only the debug/symbol tables nothing at runtime consults go. Measured: -23.2%
# (566699 -> 435200 bytes), bit-identical output.
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s")

# What is deliberately NOT here, all of it measured by opt-experiment.sh rather than assumed:
#
#   -ffunction-sections / -fdata-sections / -Wl,--gc-sections
#       Used to sit on CMAKE_SHARED_LINKER_FLAGS (a plain, unconditional linker-flags
#       variable that used to be set here) - i.e. only at the link stage. The first two are
#       compiler code-gen flags with nothing to do at link time; -Wl,--gc-sections did reach
#       the linker, but with nothing split per-function/variable to collect, the whole trio
#       was dead weight (confirmed via compile_commands.json: none of the three appeared in a
#       single .cpp.obj compile command) - the same shape of bug swe-jni-lib found in its own
#       flags, just via a different CMake variable.
#       Actually fixing the placement was measured, not assumed to help: gcal-11j exports
#       exactly one JNI entry point (unlike swe's 106), and reading the source finds real dead
#       code behind it - CalcEvents, CalcMasaList, ConditionEvaluate,
#       TFinderBuffer::CalculateFindCalendar, CalcEndDate, and everything TResultMasaList /
#       TResultEvents touch are defined but never called from gcal.cpp's query dispatch (see
#       FINDINGS.md) - so a bigger win than swe-jni-lib's measured -9.7% looked plausible.
#       Measured the opposite: fixing the placement makes the DLL +26.9% bigger (718904 vs
#       566699 bytes before stripping) - worse than swe-jni-lib's already-negative result. The
#       per-section alignment padding this codebase's many small functions pay costs more than
#       any dead code recovers. Left out entirely - being dead was accidentally the right
#       amount of "on".
#   -O2
#       -3.0% smaller than the CMAKE_CXX_FLAGS_RELEASE default (-O3 -DNDEBUG, supplied by
#       CMake itself) but not measurably faster (-0.1%, within run-to-run noise) - no measured
#       reason to diverge from swe-jni-lib's -O3 baseline.
#   -ffast-math / -Ofast / -march= / -mfma
#       Change the numbers, full stop - not used by anything that pins a digit today, but the
#       same -ffp-contract=fast risk documented in swe-jni-lib/CMakeLists.txt applies here:
#       plain x86-64 has no FMA, which is the only reason a future -march= bump would need
#       re-measuring rather than assuming safe.
#
# Every variant above reproduced the identical fingerprint 43951d39e5a89dbd
# (tools/GBench.java, folding the UTF-8 bytes of every q=calendar / q=sankranti / q=naksatra /
# q=tithi response across three locations and six years spanning 1950-2099). A DLL built with
# -O3 -DNDEBUG -fno-math-errno -s and nothing else measured 435200 bytes, -23.2% versus the
# 566699-byte baseline this file used to produce, with no change in output or measured speed.

if(NOT DEFINED ENV{JAVA_HOME})
    message(FATAL_ERROR "System Variable is not defined: JAVA_HOME")
endif()

set(JNI_INCLUDE "$ENV{JAVA_HOME}/include")
set(JNI_INCLUDE_WIN32 "$ENV{JAVA_HOME}/include/win32")

include_directories(${JNI_INCLUDE})
include_directories(${JNI_INCLUDE_WIN32})

add_library(gcal-11j SHARED jni/apc.cpp jni/astro.cpp jni/astro_arr.cpp jni/astro_base.cpp jni/astro_cr.cpp jni/astro_moon.cpp jni/astro_ms.cpp jni/astro_res.cpp jni/astro_sun.cpp jni/astro_w.cpp jni/avc.cpp jni/gcal.cpp jni/gc_fb.cpp jni/gc_gstr.cpp jni/gc_ui.cpp jni/gmath.cpp jni/tfile.cpp jni/tstring.cpp jni/ttimezone.cpp)