#ifndef _GCAL_PLATFORM_HDR__
#define _GCAL_PLATFORM_HDR__

//#include <android/log.h>
//#define GCAL_LOG_TAG "GCAL11j3"

//#define GCAL_ILOG(...) __android_log_print(ANDROID_LOG_INFO, GCAL_LOG_TAG, __VA_ARGS__)
//#define GCAL_DLOG(...) __android_log_print(ANDROID_LOG_DEBUG, GCAL_LOG_TAG, __VA_ARGS__)
//#define GCAL_ELOG(...) __android_log_print(ANDROID_LOG_ERROR, GCAL_LOG_TAG, __VA_ARGS__)
//#define GCAL_VLOG(...) __android_log_print(ANDROID_LOG_VERBOSE, GCAL_LOG_TAG, __VA_ARGS__)

//#ifdef WIN32
typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef char SInt8;
typedef short SInt16;
typedef int SInt32;
typedef bool Boolean;
//#endif

#ifdef _MSC_VER
	#define stricmp _stricmp
	#define strnicmp _strnicmp
#else
	#define stricmp strcasecmp
	#define strnicmp strncasecmp
#endif


#endif
