// TLocation.h: interface for the TLocation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TLOCATION_H__1A20E2CD_B98A_47EC_B2F7_7CD68C11DBCB__INCLUDED_)
#define AFX_TLOCATION_H__1A20E2CD_B98A_47EC_B2F7_7CD68C11DBCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PORTABLE

class TLocation  
{
public:
	static TLocation gloc[];
	static int GetLocationCount();

	const char * name;
	float latitude;
	float longitude;
	unsigned short country_code;
	unsigned short timezone_id;

};

extern TLocation gloc[];

#endif // !defined(AFX_TLOCATION_H__1A20E2CD_B98A_47EC_B2F7_7CD68C11DBCB__INCLUDED_)
