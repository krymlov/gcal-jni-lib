// VCal5beta.cpp : Defines the class behaviors for the application.
//

#include <stdlib.h>

#include "platform.h"
#include "avc.h"
#include "apc.h"
#include "tfile.h"
#include "ttimezone.h"

Boolean GCalApp_InitInstance();//char * args[], int argn);
int GCalApp_InitFolders();
void GCalApp_InitInstanceData();
const char * GCalApp_GetFileName(int n);


extern int g_dstSelMethod;
extern CLocation gMyLocation;
extern CLocation gLastLocation;
extern VCTIME gToday;
extern VCTIME gTomorrow;
extern VCTIME gYesterday;
extern int g_BhanuMode;
extern TLangFileList gLangList;


void GCalApp_InitInstanceData()
{
/*	gMyLocation.m_fLongitude = 75.766600;
	gMyLocation.m_fLatitude = 23.183333;
	gMyLocation.m_fTimezone = 5.5;
	//gMyLocation.m_strLatitude = "23N10";
	//gMyLocation.m_strLongitude = "75E45";
	gMyLocation.m_strCity = "Vrindavan";
	gMyLocation.m_strCountry = "India";
	//gMyLocation.m_strTimeZone = "+5:30";
	gMyLocation.m_nDST = 188;
	gLastLocation.m_fLongitude = 75.766600;
	gLastLocation.m_fLatitude = 23.183333;
	gLastLocation.m_fTimezone = 5.5;
	//gLastLocation.m_strLatitude = "23°10' N";
	//gLastLocation.m_strLongitude = "75°45' E";
	gLastLocation.m_strCity = "Vrindavan";
	gLastLocation.m_strCountry = "India";
	//gLastLocation.m_strTimeZone = "5h 30min";
	gLastLocation.m_nDST = 188;

	TString strFile;
	TFile f;
	TString str, strA, strB;
	TLangFileInfo * pl;

	if ( f.Open(GCalApp_GetFileName(GSTR_CONF_FILE), "r") == false)
		goto _read_locations;

	// nacitava
	while(f.ReadString2(strA, strB))
	{
		switch(atoi(strA.c_str()))
		{
		case 1:
			gMyLocation.m_fLongitude = atof(strB);
			break;
		case 2:
			gMyLocation.m_fLatitude = atof(strB);
			break;
		case 3:
			gMyLocation.m_fTimezone = atof(strB);
			break;
//		case 4:
//			gMyLocation.m_strLatitude = strB;
//			break;
//		case 5:
//			gMyLocation.m_strLongitude = strB;
//			break;
		case 6:
			gMyLocation.m_strCity = strB;
			break;
		case 21:
			gMyLocation.m_strCountry = strB;
			break;
		case 8:
			gMyLocation.m_nDST = atoi(strB);
			break;
		case 11:
			gLastLocation.m_fLongitude = atof(strB);
			break;
		case 12:
			gLastLocation.m_fLatitude = atof(strB);
			break;
		case 13:
			gLastLocation.m_fTimezone = atof(strB);
			break;
		case 16:
			gLastLocation.m_strCity = strB;
			break;
		case 22:
			gLastLocation.m_strCountry = strB;
			break;
		case 18:
			gLastLocation.m_nDST = atoi(strB);
			break;
		case 20:
			// meno subora s jazykom
			pl = gLangList.add();
			if (pl) pl->decode(strB.c_str());
			break;
		default:
			break;
		}
	}
	
	f.Close();

	// inicializacia jazyka
//	InitLanguageOutputFromFile(m_strLangFile);

_read_locations:
	////////////////////////////////////////////////////////
	//
	// inicializacia miest a kontinentov
	//
	theLocs.InitList(GCalApp_GetFileName(GSTR_LOC_FILE));

	////////////////////////////////////////////////////////
	//
	// inicializacia zobrazovanych nastaveni
	//
	AvcShowSetReadFile(GCalApp_GetFileName(GSTR_SSET_FILE));

	////////////////////////////////////////////////////////
	//
	// inicializacia custom events
	//
	CustomEventListReadFile(GCalApp_GetFileName(GSTR_CE_FILE));
*/
}

void GCalApp_ExitInstance() 
{
/*	TString str;
	TLangFileInfo * p;
	TString strFile;
	TFile f;
	
	if (f.Open(GCalApp_GetFileName(GSTR_CONF_FILE), "w") == false)
		goto _next_file_to_save;

	f.WriteString2( 1, gMyLocation.m_fLongitude);
	f.WriteString2( 2, gMyLocation.m_fLatitude);
	f.WriteString2( 3, gMyLocation.m_fTimezone);
//	f.WriteString2( 4, gMyLocation.m_strLatitude);
//	f.WriteString2( 5, gMyLocation.m_strLongitude);
	f.WriteString2( 6, gMyLocation.m_strCity);
	f.WriteString2(21, gMyLocation.m_strCountry);
//	f.WriteString2( 7, gMyLocation.m_strTimeZone);
	f.WriteString2( 8, gMyLocation.m_nDST);
	f.WriteString2(11, gLastLocation.m_fLongitude);
	f.WriteString2(12, gLastLocation.m_fLatitude);
	f.WriteString2(13, gLastLocation.m_fTimezone);
//	f.WriteString2(14, gLastLocation.m_strLatitude);
//	f.WriteString2(15, gLastLocation.m_strLongitude);
	f.WriteString2(16, gLastLocation.m_strCity);
	f.WriteString2(22, gLastLocation.m_strCountry);
//	f.WriteString2(17, gLastLocation.m_strTimeZone);
	f.WriteString2(18, gLastLocation.m_nDST);
	for(p = gLangList.list; p != NULL; p = p->next)
	{
		p->encode(str);
		f.WriteString2(20, str);
	}
	f.Close();


_next_file_to_save:
	if (theLocs.m_bModified)
	{
		theLocs.SaveAs(GCalApp_GetFileName(GSTR_LOC_FILE), 3);
	}

	AvcShowSetWriteFile(GCalApp_GetFileName(GSTR_SSET_FILE));
*/
}

Boolean GCalApp_InitLanguageOutputFromFile(const char * pszFile)
{
/*	TFile f;
	TString strA, strB;

	if (f.Open(pszFile, "r") == true)
	{
		int n;

		while(f.ReadString2(strA, strB))
		{
			n = atoi(strA);
			if ((n >= 0) && (n < 800))
			{
				gstr[n] = strB;
			}
		}

		f.Close();

		return true;
	}
*/
	return false;
}

Boolean GCalApp_GetLangFileForAcr(const char * pszAcr, TString &strFile)
{
/*	TLangFileInfo * p = gLangList.list;

	while(p)
	{
		if (p->m_strAcr.CompareNoCase(pszAcr) == 0)
		{
			strFile = p->m_strFile;
			return true;
		}
		p = p->next;
	}*/
	return false;
}

Boolean GCalApp_ParseCommandArguments(int argc, char * args[])
{
	/****************************/
	/*                          */
	/*                          */
	/*                          */
	/*                          */
	/*                          */
	/*                          */
	/*                          */
	/*                          */
	/*                          */
	/****************************/

	
	
	return false;

	
	
	
	
	
	// if no arguments, then application should open window
	if (argc < 2)
		return false;

	CLocation loc;
	VCTIME vcStart, vcEnd;
	VATIME vaStart, vaEnd;
	int nCount;
	int nReq;
	TString str;
	TString strFileOut;
	FILE * fout = stdout;

	loc.m_fLatitude = 0.0;
	loc.m_fLongitude = 0.0;
	loc.m_fTimezone = 0.0;
	loc.m_nDST = 0;
	loc.m_strCity = "";
	loc.m_strCountry = "";
	vcStart.day = 0;
	vcStart.month = 0;
	vcStart.year = 0;
	vcEnd = vcStart;
	vaStart.tithi = vaStart.masa = vaStart.gyear = 0;
	vaEnd = vaStart;
	nCount = -1;

	for(int i = 1; i < argc; i++)
	{
		//TRACE2("arg %d = %s\n", i, args[i]);
		if (strcmp(args[i],"-L") == 0)
		{
			if (argc >= i+2)
			{
//				loc.m_strLongitude = args[i+1];
				ApcString_GetArg_EarthPos(args[i+1], loc.m_fLatitude, loc.m_fLongitude);
				//TRACE2("-L latitude=%f longitude=%f\n", loc.m_fLatitude, loc.m_fLongitude);
			}
			i++;
		}
		else if (strcmp(args[i],"-N") == 0)
		{
			if (argc >= i+2)
			{
				loc.m_strCity = args[i+1];
				//TRACE1("-N name=%s\n", loc.m_strName);
			}
			i++;
		}
		else if (strcmp(args[i],"-SV") == 0)
		{
			if (argc >= i+2)
			{
				ApcString_GetArg_VaisnDate(args[i+1], vaStart);
			}
			i++;
		}
		else if (strcmp(args[i],"-SG") == 0)
		{
			if (argc >= i+2)
			{
				ApcString_GetArg_Date(args[i+1], vcStart);
			}
			i++;
		}
		else if (strcmp(args[i],"-ST") == 0)
		{
			if (argc >= i+2)
			{
				ApcString_GetArg_Time(args[i+1], vcStart);
			}
			i++;
		}
		else if (strcmp(args[i],"-EG") == 0)
		{
			if (argc >= i+2)
			{
				ApcString_GetArg_Date(args[i+1], vcEnd);
				//AfxTrace("-EG day=%d month=%d year=%d\n", vcEnd.day, vcEnd.month, vcEnd.year);
			}
			i++;
		}
		else if (strcmp(args[i],"-EV") == 0)
		{
			if (argc >= i+2)
			{
				ApcString_GetArg_VaisnDate(args[i+1], vaEnd);
				//AfxTrace("-EV tithi=%d masa=%d gyear=%d\n", vaEnd.tithi, vaEnd.masa, vaEnd.gyear);
			}
			i++;
		}
		else if (strcmp(args[i],"-EC") == 0)
		{
			if (argc >= i+2)
			{
				nCount = atoi(args[i+1]);
				//AfxTrace("couint = %d\n", nCount);
			}
			i++;
		}
		else if (strcmp(args[i],"-TZ") == 0)
		{
			if (argc >= i+2)
			{
				ApcString_GetArg_TimeZone(args[i+1], loc.m_fTimezone);
				//TRACE1("-TZ  timezone=%f\n", loc.m_fTimezone);
			}
			i++;
		}
		else if (strcmp(args[i],"-LNG") == 0)
		{
			if (argc >= i+2)
			{
				TString strLang;
				strLang = args[i+1];
				TString strFile;
				if (GCalApp_GetLangFileForAcr(strLang, strFile) == true)
				{
					GCalApp_InitLanguageOutputFromFile(strFile);
				}
			}
			i++;
		}
		else if (strcmp(args[i],"-DST") == 0)
		{
			if (argc >= i+2)
			{
				loc.m_nDST = TTimeZone::GetID(args[i+1]);
			}
			i++;
		}
		else if (strcmp(args[i],"-O") == 0)
		{
			if (argc >= i+2)
			{
				if (fout != stdout && fout != stderr)
					fclose(fout);
				fout = fopen(args[i+1], "wt");
				//strFileOut = args[i+1];
				if (fout == NULL)
					fout = stderr;
			}
			i++;
		}
		else if (stricmp(args[i],"-R") == 0)
		{
			if (argc >= i+2)
			{
				if (stricmp(args[i + 1], "calendar") == 0)
				{
					nReq = 10;
				} else if (stricmp(args[i + 1], "appday") == 0)
				{
					nReq = 11;
				} else if (stricmp(args[i + 1], "tithi") == 0)
				{
					nReq = 12;
				} else if (stricmp(args[i + 1], "sankranti") == 0)
				{
					nReq = 13;
				} else if (stricmp(args[i + 1], "naksatra") == 0)
				{
					nReq = 14;
				} else if (stricmp(args[i + 1], "firstday") == 0)
				{
					nReq = 15;
				} else if (stricmp(args[i + 1], "gcalendar") == 0)
				{
					nReq = 16;
				} else if (stricmp(args[i + 1], "gtithi") == 0)
				{
					nReq = 17;
				} else if (stricmp(args[i + 1], "next") == 0)
				{
					nReq = 18;
				} else if (stricmp(args[i + 1], "xlan") == 0)
				{
					nReq = 50;
				}
				else if (stricmp(args[i + 1], "help") == 0)
				{
					nReq = 60;
				}
				/*else if (stricmp(args[i + 1], "") == 0)
				{
				} else if (stricmp(args[i + 1], "") == 0)
				{
				} else if (stricmp(args[i + 1], "") == 0)
				{
				} else if (stricmp(args[i + 1], "") == 0)
				{
				}*/
			}
			i++;
		}
	}

	vcStart.tzone = loc.m_fTimezone;
	vcEnd.tzone = loc.m_fTimezone;
	//AfxTrace("vcStart = %d,%d,%d,...,%f\n", vcStart.day, vcStart.month, vcStart.year, vcStart.shour);

	switch(nReq)
	{
	case 10:
	case 13:
	case 14:
		if (vcStart.year == 0 && vaStart.gyear != 0)
			VATIMEtoVCTIME(vaStart, vcStart, (EARTHDATA)loc);
		if (vcEnd.year == 0 && vaEnd.gyear != 0)
			VATIMEtoVCTIME(vaEnd, vcEnd, (EARTHDATA)loc);		
		break;
	default:
		break;
	}

	if (vcStart.year != 0 && vcEnd.year != 0 && nCount < 0)
		nCount = int(vcEnd.GetJulian() - vcStart.GetJulian());

	if (nCount < 0)
		nCount = 30;

	TResultApp appday;
	TResultCalendar calendar;
	//AfxTrace("Count === %d\n", nCount);

	switch(nReq)
	{
	case 10:
		// -R -O -LAT -LON -SG -C [-DST -NAME]
		vcStart.NextDay();
		vcStart.PreviousDay();
		CalcCalendar(calendar, loc, vcStart, nCount);
		WriteCalendarXml(calendar, fout);
		break;
	case 11:
		// -R -O -LAT -LON -SG -ST [-NAME]
		CalcAppDay(loc, vcStart, appday);
		FormatAppDayXML(appday, str);
		fputs(str, fout);
		break;
	case 12:
		WriteXML_Tithi( fout, loc, vcStart);
		break;
	case 13:
		if (vcEnd.year == 0)
		{
			vcEnd = vcStart;
			vcEnd += nCount;
		}
		WriteXML_Sankrantis(fout, loc, vcStart, vcEnd);
		break;
	case 14:
		WriteXML_Naksatra(fout, loc, vcStart, nCount);
		break;
	case 15:
		WriteXML_FirstDay_Year(fout, loc, vcStart);
		break;
	case 16:
		vcStart = GetFirstDayOfYear((EARTHDATA)loc, vcStart.year);
		vcEnd = GetFirstDayOfYear((EARTHDATA)loc, vcStart.year + 1);
		nCount = int(vcEnd.GetJulian() - vcStart.GetJulian());
		CalcCalendar(calendar, loc, vcStart, nCount);
		WriteCalendarXml(calendar, fout);
		break;
	case 17:
		WriteXML_GaurabdaTithi( fout, loc, vaStart, vaEnd);
		break;
	case 18:
		WriteXML_GaurabdaNextTithi( fout, loc, vcStart, vaStart);
		break;
	case 50:
		{
			int i = 0;
			TString str;
			fputs("10000\nEnglish\n10001\nen\n", fout);
			// export obsahu do subora
			for(i = 0; i < 900; i ++)
			{
				if (!gstr[i].IsEmpty())
				{
					str.Format("%d\n%s\n", i, gstr[i].c_str());
					fputs(str, fout);
				}
			}
		}
		break;
	}

	return true;
}

Boolean GCalApp_InitInstance()//char * args[], int argn)
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	
	InitGlobalStrings();

	// initialization for AppDir
	GCalApp_InitFolders();

	g_BhanuMode = 0;

	//if (GCalApp_ParseCommandArguments(argn, args) == true)
	//	return false;

	GCalApp_InitInstanceData();

	return true;
}

TString g_appstr[32];

int mkdir(const char * s)
{
	TString T;

	T = "mkdir ";
	T += s;

	system(T.c_str());

	return 1;
}

int GCalApp_InitFolders()
{
/*	char pszBuffer[1024];
	pszBuffer[0] = 0;

	g_appstr[GSTR_APPFOLDER] = pszBuffer;

	g_appstr[GSTR_CONFOLDER] = g_appstr[GSTR_APPFOLDER];
	//g_appstr[GSTR_CONFOLDER] += "config\\";
//	mkdir(g_appstr[GSTR_CONFOLDER].c_str());

	g_appstr[GSTR_LANFOLDER] = g_appstr[GSTR_APPFOLDER];
	//g_appstr[GSTR_LANFOLDER] += "lang\\";
//	mkdir(g_appstr[GSTR_LANFOLDER].c_str());

	g_appstr[GSTR_TEMFOLDER] = g_appstr[GSTR_APPFOLDER];
	//g_appstr[GSTR_TEMFOLDER] += "temp\\";
//	mkdir(g_appstr[GSTR_TEMFOLDER].c_str());

	g_appstr[GSTR_CE_FILE].Format("%scevents.cfg", g_appstr[GSTR_CONFOLDER].c_str());
	g_appstr[GSTR_CONF_FILE].Format("%scurrent.cfg", g_appstr[GSTR_CONFOLDER].c_str());
	g_appstr[GSTR_LOC_FILE].Format("%slocations.cfg", g_appstr[GSTR_CONFOLDER].c_str());
	g_appstr[GSTR_SSET_FILE].Format("%sshowset.cfg", g_appstr[GSTR_CONFOLDER].c_str());
*/
	return 1;
}

const char * GCalApp_GetFileName(int n)
{
	return g_appstr[n].c_str();
}
