// XmlFile.h: interface for the TFileXml class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLFILE_H__58C81085_95B4_4F0A_9E95_6F2AFD75ADA0__INCLUDED_)
#define AFX_XMLFILE_H__58C81085_95B4_4F0A_9E95_6F2AFD75ADA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PORTABLE

#include "astro.h"
;
class TFileXml  
{
	FILE * fout;
public:
	void operator=(FILE * file) { fout = file; }
	const char * FormatInput(VCTIME vc,char *XMLOut);
	const char * FormatInput(TString &s,char *XMLOut);	
	const char * FormatInput(DAYTIME dt,char *XMLOut);
	const char * FormatInput(int n,char *XMLOut);
	const char * FormatInput(double d,char *XMLOut);	
	TFileXml();	
	virtual ~TFileXml();	
	FILE *stream;	

};

#endif // !defined(AFX_XMLFILE_H__58C81085_95B4_4F0A_9E95_6F2AFD75ADA0__INCLUDED_)
