@echo off
rem Builds x64\Release\gcal-11j.dll (the JNI library, 64-bit) with MSVC instead of
rem MinGW (CMakeLists.txt): the same 19 sources of jni\, the options of the
rem swisseph-2026 and swe-jni-lib builds.
rem
rem   build_dll.bat
rem
rem Options: /O2 /GF- /Gy /MT /W3 /Gd /EHsc, /Ob2 with whole program optimization
rem /GL /LTCG and profile guided optimization, /Brepro; x64 (/MACHINE:X64): the JVMs
rem that load this library are 64-bit. /MT links the C runtime statically: the DLL
rem imports KERNEL32.dll only. Warnings silenced: 4996 (strcpy, sprintf...), 4068
rem (#pragma GCC), 4244 4267 4305 (narrowing conversions, all over the sources).
rem
rem Steps:
rem   1. instrumented DLL (/LTCG /GENPROFILE) in obj\x64\pgo\;
rem   2. training run: pgo\GCalBench.java (every query kind for four locations and
rem      six years, through the JNI entry point) in the JVM of %%JAVA_HOME%%;
rem   3. optimized DLL from the same objects (/LTCG /USEPROFILE) in x64\Release\;
rem   4. reference DLL, plain /O2 /Ob1 without /GL, in obj\x64\plain\;
rem   5. check: both workloads of pgo\GCalBench.java (timed and verify) must give
rem      the same checksums on the optimized and on the reference DLL.
rem
rem   set NOPGO=1 & build_dll.bat   - plain /O2 /Ob1 DLL into x64\Release\, no training, no check
rem
rem Needs: Visual Studio 2026 (18) Build Tools with the C++ x64 tools; JAVA_HOME
rem (jni.h, and java.exe for the training run, JDK 11+).
rem Run from PowerShell or cmd (not Git Bash): cmd /c "<full path>\build_dll.bat"
rem ASCII only in this file: cmd reads it in the OEM code page.
setlocal
set "LIB_NAME=gcal-11j"
set "SRC=%~dp0jni"
set "OBJ=%~dp0obj\x64"
set "OUT=%~dp0x64\Release"
set "TRAIN=%~dp0pgo\GCalBench.java"
set "VS=C:\Program Files (x86)\Microsoft Visual Studio"
set "PATH=%PATH%;%VS%\Installer"
set "FILES=apc.cpp astro.cpp astro_arr.cpp astro_base.cpp astro_cr.cpp astro_moon.cpp astro_ms.cpp astro_res.cpp astro_sun.cpp astro_w.cpp avc.cpp gcal.cpp gc_fb.cpp gc_gstr.cpp gc_ui.cpp gmath.cpp tfile.cpp tstring.cpp ttimezone.cpp"
set "OBJS=apc.obj astro.obj astro_arr.obj astro_base.obj astro_cr.obj astro_moon.obj astro_ms.obj astro_res.obj astro_sun.obj astro_w.obj avc.obj gcal.obj gc_fb.obj gc_gstr.obj gc_ui.obj gmath.obj tfile.obj tstring.obj ttimezone.obj"
set "CFLAGS=/nologo /c /O2 /GF- /Gy /MT /W3 /Gd /EHsc /wd4996 /wd4068 /wd4244 /wd4267 /wd4305 /Brepro /D NDEBUG /D _WINDOWS /D _USRDLL"
set "LFLAGS=/nologo /DLL /Brepro /SUBSYSTEM:WINDOWS /MACHINE:X64"

if "%JAVA_HOME%"=="" (echo JAVA_HOME is not set & exit /b 1)
if not exist "%JAVA_HOME%\include\jni.h" (echo no jni.h in %JAVA_HOME%\include & exit /b 1)
set "INCL=/I "%JAVA_HOME%\include" /I "%JAVA_HOME%\include\win32""
call "%VS%\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul || exit /b 1
if not exist "%OBJ%\pgo" mkdir "%OBJ%\pgo"
if not exist "%OBJ%\plain" mkdir "%OBJ%\plain"
if not exist "%OUT%" mkdir "%OUT%"
del /q "%OBJ%\*.obj" "%OBJ%\pgo\*.*" "%OBJ%\plain\*.*" 2>nul

if "%NOPGO%"=="1" goto plain

if not exist "%JAVA_HOME%\bin\java.exe" (echo no java.exe in %JAVA_HOME%\bin: needed for the training run & exit /b 1)
pushd "%SRC%"
cl %CFLAGS% %INCL% /Ob2 /GL /Fo"%OBJ%\\" %FILES% || (popd & exit /b 1)
popd
pushd "%OBJ%"
link %LFLAGS% /LTCG /GENPROFILE /PGD:pgo\%LIB_NAME%.pgd /OUT:pgo\%LIB_NAME%.dll /IMPLIB:pgo\%LIB_NAME%.lib %OBJS% || (popd & exit /b 1)
popd
"%JAVA_HOME%\bin\java.exe" "%TRAIN%" "%OBJ%\pgo\%LIB_NAME%.dll" 1 || exit /b 1
if not exist "%OBJ%\pgo\%LIB_NAME%!1.pgc" (echo the training run wrote no profile & exit /b 1)
pushd "%OBJ%"
link %LFLAGS% /LTCG /USEPROFILE /PGD:pgo\%LIB_NAME%.pgd /OUT:"%OUT%\%LIB_NAME%.dll" /IMPLIB:pgo\%LIB_NAME%.opt.lib %OBJS% || (popd & exit /b 1)
popd

rem reference: plain /O2 /Ob1, separate objects
pushd "%SRC%"
cl %CFLAGS% %INCL% /Ob1 /Fo"%OBJ%\plain\\" %FILES% || (popd & exit /b 1)
popd
pushd "%OBJ%\plain"
link %LFLAGS% /OUT:%LIB_NAME%.dll /IMPLIB:%LIB_NAME%.lib %OBJS% || (popd & exit /b 1)
popd

call :sums "%OUT%\%LIB_NAME%.dll" "%OBJ%\check_opt.txt" || exit /b 1
call :sums "%OBJ%\plain\%LIB_NAME%.dll" "%OBJ%\check_plain.txt" || exit /b 1
echo optimized:
type "%OBJ%\check_opt.txt"
echo reference (plain /Ob1):
type "%OBJ%\check_plain.txt"
fc /b "%OBJ%\check_opt.txt" "%OBJ%\check_plain.txt" >nul || (echo CHECK FAILED: the optimized DLL gives other results than the plain build & exit /b 1)
echo OK: %OUT%\%LIB_NAME%.dll (PGO; same checksums as the plain build)
exit /b 0

:plain
pushd "%SRC%"
cl %CFLAGS% %INCL% /Ob1 /Fo"%OBJ%\\" %FILES% || (popd & exit /b 1)
popd
pushd "%OBJ%"
link %LFLAGS% /OUT:"%OUT%\%LIB_NAME%.dll" /IMPLIB:plain\%LIB_NAME%.lib %OBJS% || (popd & exit /b 1)
popd
echo OK: %OUT%\%LIB_NAME%.dll (plain /O2 /Ob1, no PGO)
exit /b 0

rem :sums <dll> <file> - both checksums of pgo\GCalBench.java, without the timings
:sums
"%JAVA_HOME%\bin\java.exe" "%TRAIN%" "%~1" 1 > "%~2.run" || exit /b 1
"%JAVA_HOME%\bin\java.exe" "%TRAIN%" "%~1" 1 verify >> "%~2.run" || exit /b 1
(for /f "usebackq tokens=1,2" %%a in ("%~2.run") do echo %%a %%b) > "%~2"
exit /b 0
