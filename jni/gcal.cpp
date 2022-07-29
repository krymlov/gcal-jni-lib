// gcalc.cpp : Defines the entry point for the console application.

#include "ttimezone.h"
#include "math.h"
#include "astro.h"
#include "avc.h"

#include <stdio.h>
#include "platform.h"
#include "ctype.h"
#include "stdlib.h"
#include "time.h"

#include "gcal.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

extern int g_BhanuMode;
int AvcComboMasaToMasa(int);

Boolean GCalApp_InitInstance()
{	
	InitGlobalStrings();
	g_BhanuMode = 0;
	return true;
}

int htoi(char s) {
	if (isdigit(s)) return int(s - '0');
	if (s >= 'a' && s <= 'f') return int(s - 'a' + 10);
	if (s >= 'A' && s <= 'F') return int(s - 'A' + 10);
	return 0;
}

void DecodeQueryArgument(char * in_p) {
	char * p = in_p;
	char * d = in_p;

	while (*p) {
		if (*p == '%') {
			*d = htoi(p[1]) * 16 + htoi(p[2]);
			p += 2;
		} else if (*p == '+') {
			*p = ' ';
		}

		p++;
		d++;
	}
	*d = 0;
}

double EarthPosToDouble(const char * str) {
	double l = 0.0;
	double sig = 1.0;
	double coma = 10.0;
	bool after_coma = false;
	bool is_deg = false;
	const char * s = str;

	while(*s)
	{
		switch(*s)
		{
		case '0': case '1':
		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			if (after_coma)
			{
				if (is_deg)
				{
					l += (*s - '0') * 5.0 / (coma * 3.0);
				}
				else
				{
					l += (*s - '0') / coma;
				}
				coma *= 10.0;
			}
			else
			{
				l = l*10.0 + (*s - '0');
			}
			break;
		case 'n': case 'N':
			after_coma = true;
			is_deg = true;
			sig = 1.0;
			break;
		case 's': case 'S':
			after_coma = true;
			is_deg = true;
			sig = -1.0;
			break;
		case 'e': case 'E':
			after_coma = true;
			is_deg = true;
			sig = 1.0;
			break;
		case 'w': case 'W':
			after_coma = true;
			is_deg = true;
			sig = -1.0;
			break;
		case '.':
			after_coma = true;
			is_deg = false;
			break;
		case '-':
			sig = -1.0;
			break;
		case '+':
			sig = 1.0;
			break;
		default:
			break;
		}
		s++;
	}

	return l * sig;
}

double EarthPosToDouble2(const char *str) {
	double l = 0.0;
	double sig = 1.0;
	double coma = 10.0;
	bool after_coma = false;
	bool is_deg = false;
	const char * s = str;

	while (*s) {
		switch (*s) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (after_coma) {
				if (is_deg) {
					l += (*s - '0') * 5.0 / (coma * 3.0);
				} else {
					l += (*s - '0') / coma;
				}
				coma *= 10.0;
			} else {
				l = l * 10.0 + (*s - '0');
			}
			break;
		case 'e':
		case 'E':
		case 'n':
		case 'N':
			after_coma = true;
			is_deg = true;
			sig = 1.0;
			break;
		case 'w':
		case 'W':
		case 's':
		case 'S':
			after_coma = true;
			is_deg = true;
			sig = -1.0;
			break;
		default:
			break;
		}
		s++;
	}

	return l * sig;
}

char * szArgQuery;
char * szArgLat;
char * szArgLon;
char * szArgTz;
char * szArgName;
char * szArgYear;
char * szArgMonth;
char * szArgDay;
char * szArgHour;
char * szArgMin;
char * szArgCount;

int file_length(FILE *f) {
	int pos;
	int end;

	pos = ftell(f);
	fseek(f, 0, SEEK_END);
	end = ftell(f);
	fseek(f, pos, SEEK_SET);

	return end;
}

const char* AnalyzeQueryString(const char *mQueryString, int xmlSize) {
	//GCAL_DLOG("AnalyzeQueryString %s.", mQueryString);

	GCalApp_InitInstance();
	//XMLOut = NULL;

	char szQuery[1024];
	char * args[80][2];
	int argn, u;
	CLocation loc;
	VCTIME vc;
	int nCount = 1;
	int gy, gm, gp, gt, tye;
	EARTHDATA earth;
	int nQueryLength;

	gy = 500;
	gm = 1;
	gp = 0;
	gt = 1;
	tye = 0;

	vc.Today();
	vc.shour = 0.0;
	szArgQuery = "";
	szArgLat = "";
	szArgLon = "";
	szArgTz = "";
	szArgName = "";
	szArgYear = "";
	szArgMonth = "";
	szArgDay = "";
	szArgHour = "";
	szArgMin = "";
	szArgCount = "";

	strcpy(szQuery, mQueryString);
	nQueryLength = strlen(szQuery);
	if (nQueryLength < 5) goto lblInvalidQS;

	argn = 0;
	args[0][0] = szQuery;
	args[0][1] = szQuery;

	for (u = 0; u < nQueryLength; u++) {
		if (szQuery[u] == '&') {
			argn++;
			args[argn][0] = szQuery + u + 1;
			args[argn][1] = args[argn][0];
			szQuery[u] = 0;
		} else if (szQuery[u] == '=') {
			args[argn][1] = szQuery + u + 1;
			szQuery[u] = 0;
		}
	}

	argn++;

	for (u = 0; u < argn; u++) {
		DecodeQueryArgument(args[u][1]);
		if (strcmp(args[u][0], "la") == 0) {
			szArgLat = args[u][1];
			loc.m_fLatitude = EarthPosToDouble(szArgLat);
		} else if (strcmp(args[u][0], "q") == 0) {
			szArgQuery = args[u][1];
		} else if (strcmp(args[u][0], "lo") == 0) {
			szArgLon = args[u][1];
			loc.m_fLongitude = EarthPosToDouble(szArgLon);
		} else if (strcmp(args[u][0], "lt") == 0) {
			szArgTz = args[u][1];
			loc.m_fTimezone = EarthPosToDouble(szArgTz);
		} else if (strcmp(args[u][0], "lc") == 0) {
			szArgName = args[u][1];
			loc.m_strCity = szArgName;
		} else if (strcmp(args[u][0], "ty") == 0) {
			szArgYear = args[u][1];
			vc.year = atoi(szArgYear);
		} else if (strcmp(args[u][0], "tye") == 0) {
			tye = atoi(args[u][1]);
		} else if (strcmp(args[u][0], "tm") == 0) {
			szArgMonth = args[u][1];
			vc.month = atoi(szArgMonth);
		} else if (strcmp(args[u][0], "td") == 0) {
			szArgDay = args[u][1];
			vc.day = atoi(szArgDay);
		} else if (strcmp(args[u][0], "th") == 0) {
			szArgHour = args[u][1];
			vc.shour += atoi(szArgHour) / 24.0;
		} else if (strcmp(args[u][0], "tmin") == 0) {
			szArgMin = args[u][1];
			vc.shour += atoi(szArgMin) / 1440.0;
		} else if (strcmp(args[u][0], "tc") == 0) {
			szArgCount = args[u][1];
			nCount = atoi(szArgCount);
		} else if (strcmp(args[u][0], "gy") == 0) {
			gy = atoi(args[u][1]);
		} else if (strcmp(args[u][0], "gm") == 0) {
			gm = atoi(args[u][1]);
		} else if (strcmp(args[u][0], "gp") == 0) {
			gp = atoi(args[u][1]);
		} else if (strcmp(args[u][0], "gt") == 0) {
			gt = atoi(args[u][1]);
		} else if (strcmp(args[u][0], "dst") == 0) {
			unsigned int a[10];
			int curra = 0;
			int i;
			char szbuffa[64];
			for (i = 0; i < 10; i++) {
				a[i] = 0;
			}
			strcpy(szbuffa, args[u][1]);
			for (i = 0; curra < 10 && szbuffa[i] != 0; i++) {
				if (szbuffa[i] == 'x') curra++;
				else if (isdigit(szbuffa[i])) a[curra] = 10 * a[curra] + (szbuffa[i] - '0');
			}
			loc.m_nDST = a[0];
			loc.m_nDST = (loc.m_nDST << 6) | a[1];
			loc.m_nDST = (loc.m_nDST << 2) | a[2];
			loc.m_nDST = (loc.m_nDST << 4) | a[3];
			loc.m_nDST = (loc.m_nDST << 4) | a[4];
			loc.m_nDST = (loc.m_nDST << 6) | a[5];
			loc.m_nDST = (loc.m_nDST << 2) | a[6];
			loc.m_nDST = (loc.m_nDST << 4) | a[7];
		}
	}

	earth = (EARTHDATA) loc;

	if (strcmp(szArgQuery, "firstday") == 0) {
		//XMLOut = new char[368 * 1024];
		return WriteXML_FirstDay_Year(stdout, loc, vc);
	} else if (strcmp(szArgQuery, "appday") == 0) {
		TResultApp app;
		TString str;
		CalcAppDay(loc, vc, app);
		FormatAppDayXML(app, str);

		//int n = strlen(str.c_str());
		//char * XML = new char[n];
		//strcpy(XML, str.c_str());
		//return XML;
		return str.c_str();
	} else if (strcmp(szArgQuery, "sankranti") == 0) {
		VCTIME vc2;
		vc2 = vc;
		vc2 += nCount;

		//XMLOut = new char[xmlSize * 1024];
		return WriteXML_Sankrantis(stdout, loc, vc, vc2);
	} else if (strcmp(szArgQuery, "calendar") == 0) {
		TResultCalendar cal;
		vc.shour = 0;

		CalcCalendar(cal, loc, vc, nCount);
		//XMLOut = new char[xmlSize * 1024];

		return WriteCalendarXml(cal, stdout);
	} else if (strcmp(szArgQuery, "gcalendar") == 0) {
		VCTIME vc2;

		vc = GetFirstDayOfYear(earth, vc.year);
		vc.InitWeekDay();
		vc.shour = 0;

		vc2 = GetFirstDayOfYear(earth, vc.year + 1);
		vc2.InitWeekDay();
		vc2.shour = 0;

		TResultCalendar cal;
		vc.shour = 0;
		CalcCalendar(cal, loc, vc, int(vc2.GetJulian() - vc.GetJulian()));

		//XMLOut = new char[xmlSize * 1024];
		return WriteCalendarXml(cal, stdout);
	} else if (strcmp(szArgQuery, "naksatra") == 0) {
		//XMLOut = new char[xmlSize * 1024];
		return WriteXML_Naksatra(stdout, loc, vc, nCount);
	} else if (strcmp(szArgQuery, "gtithi") == 0) {
		VATIME va1;
		VATIME va2;
		va1.gyear = vc.year - 1486;
		va1.masa = gm;
		va1.tithi = gp * 15 + gt - 1;
		va2 = va1;
		va2.gyear = tye - 1486;

		//XMLOut = new char[xmlSize * 1024];
		return WriteXML_GaurabdaTithi( stdout, loc, va1, va2);
	} else if (strcmp(szArgQuery, "tithi") == 0) {
		//XMLOut = new char[xmlSize * 1024];
		return WriteXML_Tithi( stdout, loc, vc);
	} else if (strcmp(szArgQuery, "masastart") == 0) {
		//XMLOut = new char[xmlSize * 1024];
		return WriteXML_MasaStart( stdout, loc, vc, 0);
	} else if (strcmp(szArgQuery, "paksastart") == 0) {
		//XMLOut = new char[xmlSize * 1024];
		return WriteXML_MasaStart( stdout, loc, vc, 1);
	} else if (strcmp(szArgQuery, "next") == 0) {
		VATIME va;
		va.masa = AvcComboMasaToMasa(gm);
		va.tithi = gt - 1 + gp * 15;
		//XMLOut = new char[xmlSize * 1024];
		return WriteXML_GaurabdaNextTithi( stdout, loc, vc, va);
	} else if (strcmp(szArgQuery, "gnaksatra") == 0) {
		//XMLOut = new char[xmlSize * 1024];
		vc = GetFirstDayOfMasa(earth, gy, AvcComboMasaToMasa(gm));
		return WriteXML_Naksatra( stdout, loc, vc, nCount);
	} else {
		lblInvalidQS: return NULL;
	}

	return NULL;
}

JNIEXPORT jstring JNICALL Java_org_gaurabda_GCalManager_nativeGCal4Query(JNIEnv* env, jclass gcalManager, jstring jQuery) {
	jboolean isJQueryCopy, isJFilePathCopy;
	jstring result = NULL;

	const char* gcalQuery = env->GetStringUTFChars(jQuery, &isJQueryCopy);
	const char* xmlOut = AnalyzeQueryString(gcalQuery, 0);

	if ( NULL != xmlOut ) {
		result = env->NewStringUTF(xmlOut);
	}

	env->ReleaseStringUTFChars(jQuery, gcalQuery);
	return result;
}
