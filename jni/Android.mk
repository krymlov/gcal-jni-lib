LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := gcal-11i

LOCAL_LDFLAGS   += -ffunction-sections -fdata-sections -Wl,--gc-sections
LOCAL_CFLAGS    += -ffunction-sections -fdata-sections -fvisibility=hidden -Wall
LOCAL_CPPFLAGS  += -ffunction-sections -fdata-sections -fvisibility=hidden -Wall
LOCAL_SRC_FILES := apc.cpp astro.cpp astro_arr.cpp astro_base.cpp astro_cr.cpp astro_moon.cpp astro_ms.cpp astro_res.cpp astro_sun.cpp astro_w.cpp avc.cpp gcal.cpp gc_fb.cpp gc_gstr.cpp gc_ui.cpp gmath.cpp tfile.cpp tstring.cpp ttimezone.cpp
include $(BUILD_SHARED_LIBRARY)