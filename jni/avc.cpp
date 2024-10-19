#include <stdlib.h>

#include "astro.h"
#include "avc.h"
#include "ttimezone.h"
#include "tfile.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

//==================================================================
//
//==================================================================

int gAyaType = 1;
int gSanType = 2;
int g_BhanuMode = 0;
int g_dstSelMethod = 2;
const char * gpszSeparator = "----------------------------------------------------------------------------------";
CLocation gMyLocation;
CLocation gLastLocation;
VCTIME gTodayAct;
VCTIME gToday;
VCTIME gTomorrow;
VCTIME gYesterday;
TLangFileList gLangList;

//void SunPosition(VCTIME vct, EARTHDATA ed, SUNDATA &sun, DAYHOURS DayHours);
int is_daylight_time(VCTIME vc, int nIndex);

const char * MSG_ERROR_1 = "Incorrect format for longitude. Correct examples: 35E05, 23W45";
const char * MSG_ERROR_2 = "Incorrect format for latitude. Correct examples: 45N05, 13S45";

const char * AvcGetDSTSignature(int nDST)
{
	return nDST ? "DST" : "LT";
}

const char * AvcGetEarthPosFromString(const char * str, bool bNorth, double &Longitude)
{
	double l = 0.0;
	double sig = 1.0;
	double coma = 10.0;
	bool after_coma = false;
	bool is_deg = false;
	const char * pret = bNorth ? MSG_ERROR_1 : MSG_ERROR_2;
	bool was_comma = false;
	bool was_letter = false;
	bool was_digit = false;
	bool was_sign = false;
	int numsAfterComma = 0;

	const char * s = str;

	while(*s)
	{
		switch(*s)
		{
		case '0': case '1':
		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			was_digit = true;
			if (after_coma)
			{
				if (is_deg)
				{
					numsAfterComma++;
					if (numsAfterComma > 2)
					{
						return "Minutes are allowed only from range 0 - 59";
					}
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
				if (!bNorth)
				{
					if (l > 90.0)
						return "Latitude is allowed only in range 0 - 90";
				}
				else
				{
					if (l > 180.0)
						return "Longitude is allowed only in range 0 - 180";
				}
			}
			break;
		case 'n': case 'N':
			if (was_letter || was_sign || was_comma)
				return pret;

			was_letter = true;
			if (!bNorth)
			{
				after_coma = true;
				is_deg = true;
				sig = 1.0;
			}
			else
			{
				return pret;
			}
			break;
		case 's': case 'S':
			if (was_letter || was_sign || was_comma)
				return pret;
			was_letter = true;
			if (!bNorth)
			{
				after_coma = true;
				is_deg = true;
				sig = -1.0;
			}
			else
			{
				return pret;
			}
			break;
		case 'e': case 'E':
			if (was_letter || was_sign || was_comma)
				return pret;
			was_letter = true;
			if (bNorth)
			{
				after_coma = true;
				is_deg = true;
				sig = 1.0;
			}
			else
			{
				return pret;
			}
			break;
		case 'w': case 'W':
			if (was_letter || was_sign || was_comma)
				return pret;
			was_letter = true;
			if (bNorth)
			{
				after_coma = true;
				is_deg = true;
				sig = -1.0;
			}
			else
			{
				return pret;
			}
			break;
		case '-':
			if (was_letter || was_digit || was_comma)
				return pret;
			was_sign = true;
			sig = -1.0;
			break;
		case '+':
			if (was_letter || was_digit || was_comma)
				return pret;
			was_sign = true;
			sig = 1.0;
			break;
		case '.': case ',':
			if (was_letter || was_comma)
				return pret;
			was_comma = true;
			after_coma = true;
			is_deg = false;
			break;
		default:
			return pret;
		}
		s++;
	}

	Longitude = l * sig;

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// AppDay functions

void AddTextLine(TString &str, const char * pt)
{
	str += pt;
	str += "\r\n";
}

void AddTextLineRtf(TString &str, const char * pt)
{
	str += "\r\n\\par ";
	str += pt;
}


CShowSetting gss[] =
{
	{0, 0, "ARTI", "Tithi at arunodaya"},//0
	{0, 0, "ARTM", "Arunodaya Time"},//1
	{0, 0, "SRTM", "Sunrise Time"},//2
	{0, 0, "SSTM", "Sunset Time"},//3
	{0, 0, "MRTM", "Moonrise Time"},//4
	{0, 0, "MSTM", "Moonset Time"},//5
	{1, 1, "FEST", "Festivals"},//6
	{0, 0, "KSAY", "Info about ksaya tithi"},//7
	{0, 0, "VRDH", "Info about vriddhi tithi"},//8
	{0, 0, "SLON", "Sun Longitude"},//9
	{0, 0, "MLON", "Moon Longitude"},//10
	{0, 0, "AYAN", "Ayanamsha value"},//11
	{0, 0, "JDAY", "Julian Day"},//12
	{1, 1, "CPUR", "Caturmasya Purnima System"}, //13
	{0, 0, "CPRA", "Caturmasya Pratipat System"}, //14
	{0, 0, "CEKA", "Caturmasya Ekadasi System"}, //15
	{1, 1, "SANI", "Sankranti Info"}, //16
	{1, 1, "EKAI", "Ekadasi Info"}, //17
	{1, 1, "VHDR", "Masa Header Info"}, //18
	{0, 0, "PHDR", "Month Header Info"}, //19
	{0, 0, "EDNS", "Do not show empty days"}, //20
	{0, 0, "SBEM", "Show begining of masa"}, //21
	{1, 1, "F000", "Appearance days of the Lord"},//22
	{1, 1, "F001", "Events in the pastimes of the Lord"},//23
	{1, 1, "F002", "App, Disapp of Recent Acaryas"},//24
	{1, 1, "F003", "App, Disapp of Mahaprabhu's Associates and Other Acaryas"},//25
	{1, 1, "F004", "ISKCON's Historical Events"},//26
	{1, 1, "F005", "Bengal-specific Holidays"},//27
	{1, 1, "F006", "My Personal Events"}, //28
	/* BEGIN GCAL 1.4.3 */
	{1, 1, "TSSR", "Todat Sunrise"},  //29 Today sunrise
	{1, 1, "TSSN", "Today Noon"},  //30 today noon
	{1, 1, "TSSS", "Today Sunset"},  //31 today sunset
	{0, 0, "TSAN", "Sandhya Times"},  //32 today + sandhya times
	{1, 1, "TSIN", "Sunrise Info"},  //33 today sunrise info
	{0, 0, "ASIN", "Noon Time"},  //34 astro - noon time
	{1, 1, "NDST", "Notice about DST"}, //35 notice about the change of the DST
	{1, 1, "DNAK", "Naksatra"}, // 36 naksatra info for each day
	{1, 1, "DYOG", "Yoga"}, //37 yoga info for each day
	{1, 1, "FFLG", "Fasting Flag"},//38
	{1, 1, "DPAK", "Paksa Info"},//39 paksa info
	{0, 0, "FDIW", "First Day in Week"},//40 first day in week
	{0, 0, "DRAS", "Rasi"}, //41 moon rasi for each calendar day
	{0, 0, "OSFA", "Old Style Fasting text"}, //42 old style fasting text
	{0, 0, "MLNT", "Name of month - type"}, //43 month type name 0-vaisnava,1-bengal,2-hindu,3-vedic
	/* END GCAL 1.4.3 */
	{0, 0, "EDBL", "Editable Default Events"}, //44 editable default events
	{0, 0, "TSBM", "Today Brahma Muhurta"},     //45 brahma muhurta in today screen
	{0, 0, "TROM", "Today Rasi of the Moon"}, // 46 rasi of the moon in today screen
	{0, 0, "TNPD", "Today Naksatra Pada details"}, // 47 naksatra pada details in today screen
	{0, 0, "ADCS", "Child Names Suggestions"}, // 48 child name suggestions in Appearance Day screen
	{0, 0, "MNFO", "Masa Name Format"}, // 49 format of masa name
	{0, 0, "EPDR", "Ekadasi Parana details"}, // 50 ekadasi parana details
	{0, 0, "ANIV", "Aniversary show format"} // 51 format of aniversary info
};

int GetShowSetCount()
{
	return sizeof(gss) / sizeof(CShowSetting);
}

int GetShowSetChangesCount()
{
	int i, count = 0;
	int size = GetShowSetCount();

	for(i = 0; i < size; i++)
	{
		if (gss[i].val != gss[i].old_val)
			count++;
		gss[i].old_val = gss[i].val;
	}

	return count;
}

char * GetShowSetName(int i)
{
	return gss[i].text;
}

char * GetShowSetSignature(int i)
{
	return gss[i].sig;
}

int GetShowSetVal(int i)
{
	return gss[i].val;
}

void SetShowSetVal(int i, int val)
{
	gss[i].val = val;
	gss[i].old_val = val;
}

void SetShowSetVal(const char * pszSign, int val)
{
	int i, max = GetShowSetCount();

	for(i = 0; i < max; i++)
	{
		if (strcmp(gss[i].sig, pszSign) == 0)
		{
			gss[i].val = val;
			gss[i].old_val = val;
			break;
		}
	}
}

void AvcShowSetReadFile(const char * psz)
{
	TString strA, strB;
	TFile file;

	if (file.Open(psz, "r") == true)
	{
		while(file.ReadPropertyLine(strA, strB))
		{
			if (strA.GetLength() > 1 && strA.GetAt(0) == '#')
			{
			}
			else
			{
				SetShowSetVal(strA, atoi(strB));
			}
		}

		file.Close();
	}
}

void AvcShowSetWriteFile(const char * pszFile)
{
	TFile file;

	if (file.Open(pszFile, "w") == true)
	{
		int i, max = GetShowSetCount();
		TString str;
		for(i = 0; i < max; i++)
		{
			str.Format("#\r\n# %s\r\n#\r\n\r\n%s=%d\r\n\r\n", GetShowSetName(i), GetShowSetSignature(i),
				GetShowSetVal(i));
			file.WriteString(str);
		}
		file.Close();
	}
}

char * AvcGetTextLatitude(double d)
{
	static char inter[16];
	int a0, a1;
	char c0;

	c0 = d < 0.0 ? 'S' : 'N';
	a0 = int(fabs(d));
	a1 = int((fabs(d) - a0)*60 + 0.5);

	sprintf(inter, "%d%c%02d", a0, c0, a1);

	return inter;
}

char * AvcGetTextLongitude(double d)
{
	static char inter[16];
	int a0, a1;
	char c0;

	c0 = d < 0.0 ? 'W' : 'E';
	a0 = int(fabs(d));
	a1 = int((fabs(d) - a0)*60 + 0.5);

	sprintf(inter, "%d%c%02d", a0, c0, a1);

	return inter;
}

int AvcGetShowSetCount()
{
	return GetShowSetCount();
}

Boolean AvcGetNextShowSet(int &nItem, int &nCheck, const char * &pszText)
{
	if (nItem >= GetShowSetCount())
		return false;

	nCheck = gss[nItem].val;
	pszText = gss[nItem].text;
	nItem++;
	return true;
}

void AvcGetDateText(TString &str, VCTIME vc)
{
	if ((vc.day > 0) && (vc.day < 32) && (vc.month > 0) && (vc.month < 13) && (vc.year >= 1500) && (vc.year < 4000))
	{
		if (vc == gToday)
		{
			str.Format("%d %s %d (%s)", vc.day, AvcGetMonthAbr(vc.month), vc.year, gstr[43].c_str());
		}
		else if (vc == gTomorrow)
		{
			str.Format("%d %s %d (%s)", vc.day, AvcGetMonthAbr(vc.month), vc.year, gstr[854].c_str());
		}
		else if (vc == gYesterday)
		{
			str.Format("%d %s %d (%s)", vc.day, AvcGetMonthAbr(vc.month), vc.year, gstr[853].c_str());
		}
		else
		{
			str.Format("%d %s %d", vc.day, AvcGetMonthAbr(vc.month), vc.year);
		}
	}
}

int AvcGetNextPartHash(TString &str, TString &strLine, int & i)
{
	int index = i, length;
	
	if (index >= str.GetLength())
		return 0;

	char tc;
	strLine.Empty();
	length = str.GetLength();

	while(1)
	{
		if (index >= length)
			break;

		tc = str.GetAt(index);
		if (tc == '#')
		{
			index++;
			break;
		}
		else
		{
			strLine += tc;
		}
		index++;
	}

	i = index;
	return 1;
}

int AvcGetFestivalClass(TString &str)
{
	int i, j, val;

	i = str.Find("[c");

	if (i >= 0)
	{
//		i += 2;
		if ((i + 2) < str.GetLength())
		{
			val = int(str.GetAt(i+2) - '0');
			j = str.Find("]", i);
			if (j >= str.GetLength() || j < 0)
				j = str.GetLength();
			if (j > i)
			{
				str.Delete(i, j - i + 1);
			}
			if (val < 0 || val > 6)
				return -1;
			return val;
		}
		else
			return -1;
	}
	else
	{
		return -1;
	}

}

int AvcClearStringFromTags(TString &str)
{
	int i, j, val = 0;

	i = str.Find("[");

	while (i >= 0)
	{
		if ((i + 2) < str.GetLength())
		{
			j = str.Find("]", i);
			if (j >= str.GetLength() || j < 0)
				j = str.GetLength();
			if (j > i)
			{
				str.Delete(i, j - i + 1);
				val ++;
			}
		}

		i = str.Find("[");
	}

	return val;
}


int AvcComboMasaToMasa(int nComboMasa)
{
	return (nComboMasa == 12) ? 12 : ((nComboMasa + 11) % 12);
}

int AvcMasaToComboMasa(int nMasa)
{
	return (nMasa == 12) ? 12 : ((nMasa + 1) % 12);
}

char * AvcGetTextTimeZone(double d)
{
	int a4, a5;
	static char inter[16];

	a4 = int(d);
	a5 = int((d - a4)*60 + 0.5);

	sprintf(inter, "%d:%02d", a4, a5);

	return inter;
}

const char * AvcGetMonthAbr(int n)
{
	return gstr[64 + n].c_str();
}
char * AvcGetTimeZoneName(int n)
{
	char * tzn[] = {
    "UTC-12",
    "UTC-11",
    "UTC-10",
    "UTC-9:30",
    "UTC-9",
    "UTC-8",
    "UTC-7",
    "UTC-6",
    "UTC-5",
    "UTC-4:30",
    "UTC-4",
    "UTC-3:30",
    "UTC-3",
    "UTC-2",
    "UTC-1",
    "UTC",
    "UTC+1",
    "UTC+2",
    "UTC+3",
    "UTC+3:30",
    "UTC+4",
    "UTC+4:30",
    "UTC+5",
    "UTC+5:30",
    "UTC+5:45",
    "UTC+6",
    "UTC+6:30",
    "UTC+7",
    "UTC+8",
    "UTC+8:45",
    "UTC+9",
    "UTC+9:30",
    "UTC+10",
    "UTC+10:30",
    "UTC+11",
    "UTC+11:30",
    "UTC+12",
    "UTC+12:45",
    "UTC+13",
    "UTC+14"
	};

	if (n < 0)
		n = 0;
	if (n > 39)
		n = 39;
	return tzn[n];
}

double AvcGetTimeZone(int nIndex)
{
	double tzval[] = {
    -12.0,
    -11.0,
    -10.0,
    -9.5,
    -9.0,
    -8.0,
    -7.0,
    -6.0,
    -5.0,
    -4.5,
    -4.0,
    -3.5,
    -3.0,
    -2.0,
    -1.0,
    +0.0,
    +1.0,
    +2.0,
    +3.0,
    +3.5,
    +4.0,
    +4.5,
    +5.0,
    +5.5,
    +5.75,
    +6.0,
    +6.5,
    +7.0,
    +8.0,
    +8.75,
    +9.0,
    +9.5,
    +10.0,
    +10.5,
    +11.0,
    +11.5,
    +12.0,
    +12.75,
    +13.0,
    +14.0
	};

	if (nIndex < 0)
		nIndex = 0;
	if (nIndex > 39)
		nIndex = 39;
	return tzval[nIndex];
}

int AvcGetNearestTimeZone(double d)
{

	if (d <= AvcGetTimeZone(0))
		return 0;
	if (d >= AvcGetTimeZone(39))
		return 39;
	int n;
	for(n = 0; n < 39; n++)
	{
		if (fabs(AvcGetTimeZone(n) - d) < 0.01)
			return n;
		if ((AvcGetTimeZone(n) <= d) && (AvcGetTimeZone(n+1) >= d))
		{
			if ((d - AvcGetTimeZone(n)) < (AvcGetTimeZone(n+1) - d))
				return n;
			else
				return n+1;
		}
	}

	return 39;
}



int AvcGetLocationAsText(TString &str, EARTHDATA earth)
{
	str.Format("%s: %s  %s: %s  %s: %s", 
		gstr[10].c_str(), AvcGetTextLatitude(earth.latitude_deg), 
		gstr[11].c_str(), AvcGetTextLongitude(earth.longitude_deg),
		gstr[12].c_str(), AvcGetTextTimeZone(earth.tzone));

	return 1;
}

const char * AvcDoubleToString(double d)
{
	static char ts[128];

	sprintf(ts, "%f", d);

	return ts;
}

const char * AvcIntToString(int d)
{
	static char ts[32];

	sprintf(ts, "%d", d);

	return ts;
}

const char * AvcDateToString(VCTIME vc)
{
	static char ts[32];

	sprintf(ts, "%d %s %04d", vc.day, AvcGetMonthAbr(vc.month), vc.year);

	return ts;
}

int AvcGetNextLine(TString &str, TString &strLine, int & i)
{
	int index = i, length;
	
	if (index >= str.GetLength())
		return 0;

	char tc;
	strLine.Empty();
	length = str.GetLength();

	while(1)
	{
		if (index >= length)
			break;

		tc = str.GetAt(index);
		if (tc == '\r');
		else if (tc == '\n')
		{
			index++;
			break;
		}
		else
		{
			strLine += tc;
		}
		index++;
	}

	i = index;
	return 1;
}

Boolean ConditionEvaluate(VAISNAVADAY * pd, int nClass, int nValue, TString &strText, Boolean defaultRet)
{
	static char * pcstr[] = {"", "", "", "", "", "", "", "", "", "", 
		"[c0]", "[c1]", "[c2]", "[c3]", "[c4]", "[c5]", "", ""};

	switch(nClass)
	{
	// mahadvadasis
	case 1: if (nValue == EV_NULL)
			return ((pd->nMhdType != EV_NULL) && (pd->nMhdType != EV_SUDDHA));
		else
			return (pd->nMhdType == nValue);
	// sankrantis
	case 2: if (nValue == 0xff)
			return (pd->sankranti_zodiac >= 0);
		else
			return (pd->sankranti_zodiac == nValue);
	// tithi + paksa
	case 3: return (pd->astrodata.nTithi == nValue);
	// naksatras
	case 4: return (pd->astrodata.nNaksatra == nValue);
	// yogas
	case 5: return (pd->astrodata.nYoga == nValue);
	// fast days
	case 6: if (nValue == 0)
			return (pd->nFastType != FAST_NULL);
		else
			return (pd->nFastType == (0x200 + nValue));
	// week day
	case 7: return (pd->date.dayOfWeek == nValue);
	// tithi
	case 8: return (pd->astrodata.nTithi % 15 == nValue);
	// paksa
	case 9: return (pd->astrodata.nPaksa == nValue);
	case 10: return (pd->festivals.FindNoCase(strText.c_str()) >= 0);
	default:
		return defaultRet;
	}
}

int AvcGetTextLineCount(VAISNAVADAY * pvd)
{
	int i, nFestClass;
	int nCount = 0;
	TString str2;

	nCount++;

	if (GetShowSetVal(17) == 1)
	{
		if (pvd->ekadasi_parana)
		{
			nCount++;
		}
	}

	if (GetShowSetVal(6) == 1)
	{
		if (pvd->festivals)
		{
			i = pvd->GetHeadFestival();
			while(pvd->GetNextFestival(i, str2))
			{
				if (str2.GetLength() > 1)
				{
					nFestClass = pvd->GetFestivalClass(str2);
					if (nFestClass < 0 || GetShowSetVal(22 + nFestClass) == 1)
					{
						nCount++;
					}
				}
			}
		}
	}

	if (GetShowSetVal(16) == 1 && pvd->sankranti_zodiac >= 0)
	{
		nCount++;
	}

	if (GetShowSetVal(7) == 1 && pvd->was_ksaya)//(m_dshow.m_info_ksaya) && (pvd->was_ksaya))
	{
		nCount++;
	}

	if (GetShowSetVal(8) == 1)//(m_dshow.m_info_vriddhi) && (pvd->is_vriddhi))
	{
		if (pvd->is_vriddhi)
		{
			nCount++;
		}
	}

	if (pvd->nCaturmasya & CMASYA_CONT_MASK)
	{
		nCount++;
	}

	if ((GetShowSetVal(13) == 1) && (pvd->nCaturmasya & CMASYA_PURN_MASK))
	{
		nCount++;
		if ((pvd->nCaturmasya & CMASYA_PURN_MASK_DAY) == 0x1)
		{
			nCount++;
		}
	}

	if ((GetShowSetVal(14) == 1) && (pvd->nCaturmasya & CMASYA_PRAT_MASK))
	{
		nCount++;
		if ((pvd->nCaturmasya & CMASYA_PRAT_MASK_DAY) == 0x100)
		{
			nCount++;
		}
	}

	if ((GetShowSetVal(15) == 1) && (pvd->nCaturmasya & CMASYA_EKAD_MASK))
	{
		nCount++;
		if ((pvd->nCaturmasya & CMASYA_EKAD_MASK_DAY) == 0x10000)
		{
			nCount++;
		}
	}

	// tithi at arunodaya
	if (GetShowSetVal(0) == 1)//m_dshow.m_tithi_arun)
	{
		nCount++;
	}

	//"Arunodaya Time",//1
	if (GetShowSetVal(1) == 1)//m_dshow.m_arunodaya)
	{
		nCount++;
	}
	//"Sunrise Time",//2
	//"Sunset Time",//3
	if (GetShowSetVal(2) == 1)//m_dshow.m_sunrise)
	{
		nCount++;
	}
	if (GetShowSetVal(3) == 1)//m_dshow.m_sunset)
	{
		nCount++;

	}
	//"Moonrise Time",//4
	if (GetShowSetVal(4) == 1)
	{
		nCount++;
	}
	//"Moonset Time",//5
	if (GetShowSetVal(5) == 1)
	{
		nCount++;
	}
	///"Sun Longitude",//9
	if (GetShowSetVal(9) == 1)//m_dshow.m_sun_long)
	{
		nCount++;
	}
	//"Moon Longitude",//10
	if (GetShowSetVal(10) == 1)//m_dshow.m_sun_long)
	{
		nCount++;
	}
	//"Ayanamsha value",//11
	if (GetShowSetVal(11) == 1)//m_dshow.m_sun_long)
	{
		nCount++;
	}
	//"Julian Day",//12
	if (GetShowSetVal(12) == 1)//m_dshow.m_sun_long)
	{
		nCount++;
	}

	return nCount;
}

const char * FormatDate(VCTIME vc, VATIME va)
{
	static char sz[128];

	sprintf(sz, "%d %s %d\r\n%s, %s Paksa, %s Masa, %d",
		vc.day, AvcGetMonthAbr(vc.month), vc.year,
		GetTithiName(va.tithi%15), GetPaksaName(va.tithi/15), GetMasaName(va.masa), va.gyear);

	return sz;
}

int CalcEndDate(EARTHDATA m_earth, VCTIME vcStart, VATIME vaStart, VCTIME &vcEnd, VATIME &vaEnd, int nType, int nCount)
{
	switch(nType)
	{
	case 1:
		vcEnd = vcStart;
		if (nCount > 30240) nCount = 30240;
		vcEnd += nCount;
		VCTIMEtoVATIME(vcEnd, vaEnd, m_earth);
		break;
	case 2:
		vcEnd = vcStart;
		if (nCount > 4320) nCount = 4320;
		vcEnd += nCount*7;
		VCTIMEtoVATIME(vcEnd, vaEnd, m_earth);
		break;
	case 3:
		vcEnd = vcStart;
		if (nCount > 1080) nCount = 1080;
		vcEnd.month += nCount;
		while(vcEnd.month > 12)
		{
			vcEnd.year++;
			vcEnd.month -= 12;
		}
		VCTIMEtoVATIME(vcEnd, vaEnd, m_earth);
		break;
	case 4:
		vcEnd = vcStart;
		if (nCount > 90) nCount = 90;
		vcEnd.year += nCount;
		VCTIMEtoVATIME(vcEnd, vaEnd, m_earth);
		break;
	case 5:
		vaEnd = vaStart;
		if (nCount > 30240) nCount = 30240;
		vaEnd.tithi += nCount;
		while(vaEnd.tithi >= 30)
		{
			vaEnd.tithi-=30;
			vaEnd.masa++;
		}
		while(vaEnd.masa >= 12)
		{
			vaEnd.masa -= 12;
			vaEnd.gyear++;
		}
		VATIMEtoVCTIME(vaEnd, vcEnd, m_earth);
		break;
	case 6:
		vaEnd = vaStart;
		if (nCount > 1080) nCount = 1080;
		vaEnd.masa += nCount;
		while(vaEnd.masa >= 12)
		{
			vaEnd.masa -= 12;
			vaEnd.gyear++;
		}
		VATIMEtoVCTIME(vaEnd, vcEnd, m_earth);
		break;
	case 7:
		vaEnd = vaStart;
		if (nCount > 90) nCount = 90;
		vaEnd.gyear += nCount;
		VATIMEtoVCTIME(vaEnd, vcEnd, m_earth);
		break;
	}

	return 1;
}
