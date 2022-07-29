// TTimeZone.h: interface for the TTimeZone class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TTIMEZONE_H__B0EA8616_6B27_46C9_B778_0E2C7F07858C__INCLUDED_)
#define AFX_TTIMEZONE_H__B0EA8616_6B27_46C9_B778_0E2C7F07858C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PORTABLE
#include "platform.h"
#include "tstring.h"

class VCTIME;

class TTimeZone  
{
public:
	static int ID2INDEX(int _id);
	static int NAME2INDEX(const char * _id);
	static int GetID(const char * p);
	static int GetNormalTimeStartDate(int nDst, int nYear, VCTIME &vcStart);
	static int GetDaylightTimeStartDate(int nDst, int nYear, VCTIME &vcStart);
	static int GetTimeZoneBias(int ndst);
	static void ExpandVal(unsigned int dw, int a[]);
	static bool GetXMLString(TString &str, int nIndex);
	static int GetTimeZoneCount();
	static double GetTimeZoneOffset(int nIndex);
	static int GetTimeZoneOffsetInteger(int nIndex);
	static const char * GetTimeZoneName(int nIndex);
	static TTimeZone gzone[];

	const char * name;
	int offset;
	int bias;
	unsigned long int val;
	float latA;
	float latB;
	float lonA;
	float lonB;
	int idx;

};

#endif // !defined(AFX_TTIMEZONE_H__B0EA8616_6B27_46C9_B778_0E2C7F07858C__INCLUDED_)
