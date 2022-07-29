// TCountry.h: interface for the TCountry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCOUNTRY_H__35338704_92BC_4042_8A59_B567DDC356DE__INCLUDED_)
#define AFX_TCOUNTRY_H__35338704_92BC_4042_8A59_B567DDC356DE__INCLUDED_

#include "platform.h"

class TCountry  
{
public:
	static const char *  gcontinents[];
	static TCountry gcountries[];
	static const char * GetCountryAcronymByIndex(int nIndex);
	static const char * GetCountryNameByIndex(int nIndex);
	static int GetCountryCount();
	static const char * GetCountryContinentName(UInt16 w);
	static const char * GetCountryContinentNameByIndex(int nIndex);
	static const char * GetCountryName(UInt16 w);
	
	const char * pszAcr;
	short unsigned int code;
	const char * pszName;
	unsigned char continent;

};

#endif // !defined(AFX_TCOUNTRY_H__35338704_92BC_4042_8A59_B567DDC356DE__INCLUDED_)
