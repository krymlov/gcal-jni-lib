// CommandLineVCal.h: interface for the CCommandLineVCal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDLINEVCAL_H__2DF20853_3345_4798_B1BE_C24A2A2C656E__INCLUDED_)
#define AFX_COMMANDLINEVCAL_H__2DF20853_3345_4798_B1BE_C24A2A2C656E__INCLUDED_


#include "astro.h"
#include "platform.h"
#include "tstring.h"
// PORTABLE

int ApcString_GetArg_Interval(const char * pszText, int &A, int &B);
int ApcString_GetArg_Paksa(const char *  str);
int ApcString_GetArg_Date(const char *  str, VCTIME &vc);
int ApcString_GetArg_VaisnDate(const char *  str, VATIME &vc);
int ApcString_GetArg_Tithi(const char *  str);
int ApcString_GetArg_TimeZone(const char *  str, double &TimeZone);
int ApcString_GetArg_Time(const char *  str, VCTIME & vc);
int ApcString_GetArg_EarthPos(const char *  str, double &, double &);
int ApcString_GetArg_Masa(const char *  str);
int ApcString_GetArg_Int(const char *  str, int &nInteger);
int ApcString_GetArg_Year(const char *  str, int &nYear);
int ApcString_SetArgLastError(int i);

#endif // !defined(AFX_COMMANDLINEVCAL_H__2DF20853_3345_4798_B1BE_C24A2A2C656E__INCLUDED_)
