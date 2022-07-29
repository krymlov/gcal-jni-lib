cmake_minimum_required(VERSION 3.17)

project(gcal-11i)

set(CMAKE_CXX_STANDARD 11)

if(NOT DEFINED ENV{JAVA_HOME})
    message(FATAL_ERROR "System Variable is not defined: JAVA_HOME")
endif()

set(JNI_INCLUDE "$ENV{JAVA_HOME}/include")
set(JNI_INCLUDE_WIN32 "$ENV{JAVA_HOME}/include/win32")

include_directories(${JNI_INCLUDE})
include_directories(${JNI_INCLUDE_WIN32})

add_library(gcal-11i SHARED jni/apc.cpp jni/astro.cpp jni/astro_arr.cpp jni/astro_base.cpp jni/astro_cr.cpp jni/astro_moon.cpp jni/astro_ms.cpp jni/astro_res.cpp jni/astro_sun.cpp jni/astro_w.cpp jni/avc.cpp jni/gcal.cpp jni/gc_fb.cpp jni/gc_gstr.cpp jni/gc_ui.cpp jni/gmath.cpp jni/tfile.cpp jni/tstring.cpp jni/ttimezone.cpp)