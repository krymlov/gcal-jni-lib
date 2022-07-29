// XmlFile.cpp: implementation of the TFileXml class.
//
//////////////////////////////////////////////////////////////////////

#include "tfileex.h"
#include "stdlib.h"
#include "stdio.h"


// PORTABLE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TFileXml::TFileXml()
{	
}

TFileXml::~TFileXml()
{	
	
}



const char * TFileXml::FormatInput(TString &s,char *XMLOut)
{	
	strcat(XMLOut,s.c_str());
	return s.c_str();
}

const char * TFileXml::FormatInput(double d ,char *XMLOut)
{
	TString s;
	s.Format("%+.5f", d);		
	strcat(XMLOut,s.c_str());
	return s.c_str();
}

const char * TFileXml::FormatInput(int n,char *XMLOut)
{
	TString s;
	s.Format("%d", n);	
	strcat(XMLOut,s.c_str());
	return s.c_str();
}

const char * TFileXml::FormatInput(DAYTIME dt,char *XMLOut)
{
	TString s;
	if (dt.hour >= 0)
		s.Format("%02d:%02d:%02d", dt.hour, dt.min, dt.sec);
	else
		s = "N/A";	
	strcat(XMLOut,s.c_str());
	return s.c_str();	
}


const char *TFileXml::FormatInput(VCTIME vc,char *XMLOut)
{
	TString s;
	s.Format("%d %s %d", vc.day, AvcGetMonthAbr(vc.month), vc.year);
	strcat(XMLOut,s.c_str());
	return s.c_str();
}
