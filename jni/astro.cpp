#include "platform.h"
#include "astro.h"
#include "avc.h"
#include "ttimezone.h"
#include <time.h>
#include "stdlib.h"
// PORTABLE 

#pragma GCC diagnostic ignored "-Wwrite-strings"

void time_print(TString &str, DAYTIME dt);
int is_daylight_time(VCTIME, int);
int GetShowSetVal(int i);
void AddTextLine(TString &, const char *);

extern int g_BhanuMode;

void CalcEvents(TResultEvents &inEvents, CLocation &loc, VCTIME vcStart, VCTIME vcEnd, UInt32 fOptions)
{
	VCTIME vc;
	SUNDATA sun;
	int ndst = 0;
	int nData;

	inEvents.clear();
	inEvents.m_location = loc;
	inEvents.m_vcStart = vcStart;
	inEvents.m_vcEnd = vcEnd;
	inEvents.m_options = fOptions;
	
	VCTIME vcAdd, vcNext;
	EARTHDATA earth = (EARTHDATA)loc;

	vc = vcStart;
	vc.tzone = loc.m_fTimezone;
	vcEnd.tzone = vc.tzone;

	vcAdd = vc;
	while(vcAdd.IsBeforeThis(vcEnd))
	{
		if (fOptions & CCE_SUN)
		{
			SunCalc(vcAdd, earth, sun);

			vcAdd.shour = sun.arunodaya.GetDayTime();
			inEvents.AddEvent(vcAdd, CCTYPE_S_ARUN, 0);

			vcAdd.shour = sun.rise.GetDayTime();
			inEvents.AddEvent(vcAdd, CCTYPE_S_RISE, 0);

			vcAdd.shour = sun.noon.GetDayTime();
			inEvents.AddEvent(vcAdd, CCTYPE_S_NOON, 0);

			vcAdd.shour = sun.set.GetDayTime();
			inEvents.AddEvent(vcAdd, CCTYPE_S_SET, 0);
		}

		vcAdd.NextDay();
	}

	if (fOptions & CCE_TIT)
	{
		vcAdd = vc;
		vcAdd.shour = 0.0;
		while(vcAdd.IsBeforeThis(vcEnd))
		{
			nData = GetNextTithiStart(earth, vcAdd, vcNext);
			if (vcNext.GetDayInteger() < vcEnd.GetDayInteger())
			{
				inEvents.AddEvent(vcNext, CCTYPE_TITHI, nData);
			}
			else
			{
				break;
			}
			vcAdd = vcNext;
			vcAdd.shour += 0.2;
			if (vcAdd.shour >= 1.0)
			{
				vcAdd.shour -= 1.0;
				vcAdd.NextDay();
			}
		}
	}

	if (fOptions & CCE_NAK)
	{
		vcAdd = vc;
		vcAdd.shour = 0.0;
		while(vcAdd.IsBeforeThis(vcEnd))
		{
			nData = GetNextNaksatra(earth, vcAdd, vcNext);
			if (vcNext.GetDayInteger() < vcEnd.GetDayInteger())
			{
				inEvents.AddEvent(vcNext, CCTYPE_NAKS, nData);
			}
			else
			{
				break;
			}
			vcAdd = vcNext;
			vcAdd.shour += 0.2;
			if (vcAdd.shour >= 1.0)
			{
				vcAdd.shour -= 1.0;
				vcAdd.NextDay();
			}
		}
	}

	if (fOptions & CCE_SNK)
	{
		vcAdd = vc;
		vcAdd.shour = 0.0;
		while(vcAdd.IsBeforeThis(vcEnd))
		{
			vcNext = GetNextSankranti(vcAdd, nData);
			if (vcNext.GetDayInteger() < vcEnd.GetDayInteger())
			{
				inEvents.AddEvent(vcNext, CCTYPE_SANK, nData);
			}
			else
			{
				break;
			}
			vcAdd = vcNext;
			vcAdd.NextDay();
		}
	}

	if (fOptions & CCE_CNJ)
	{
		double dlong;
		vcAdd = vc;
		vcAdd.shour = 0.0;
		while(vcAdd.IsBeforeThis(vcEnd))
		{
			dlong = GetNextConjunction(vcAdd, vcNext, true, earth);
			if (vcNext.GetDayInteger() < vcEnd.GetDayInteger())
			{
				inEvents.AddEvent(vcNext, CCTYPE_CONJ, GetRasi(dlong, GetAyanamsa(vcNext.GetJulianComplete())) );
			}
			else
			{
				break;
			}
			vcAdd = vcNext;
			vcAdd.NextDay();
		}
	}
	
	if (fOptions & CCE_SORT)
		inEvents.Sort(true);
	else
		inEvents.b_sorted = false;
}

int FormatEventsTextXML(TResultEvents &inEvents, TString &res, TString &strXml)
{

	unsigned int i;
	TString str, temp;

	res.Format("Events from %d %s %d to %d %s %d.\r\n\r\n", 
		inEvents.m_vcStart.day,
		AvcGetMonthAbr(inEvents.m_vcStart.month),
		inEvents.m_vcStart.year,
		inEvents.m_vcEnd.day,
		AvcGetMonthAbr(inEvents.m_vcEnd.month),
		inEvents.m_vcEnd.year);

	res += inEvents.m_location.getFullName();
	res += "\r\n\r\n";

	strXml.Format("<xml>\r\n<program version=\"%s\">\r\n<location longitude=\"%f\" latitude=\"%f\" timezone=\"%f\" dst=\"%s\" />\n"
		, gstr[130].c_str(), inEvents.m_location.m_fLongitude, inEvents.m_location.m_fLatitude
		, inEvents.m_location.m_fTimezone, TTimeZone::GetTimeZoneName(inEvents.m_location.m_nDST));
	VCTIME prevd;
	int prevt = -1;

	prevd.day = 0;
	prevd.month = 0;
	prevd.year = 0;
	for(i = 0; i < inEvents.n_count; i++)
	{
		TDayEvent & dnr = inEvents[i];

		if (inEvents.b_sorted)
		{
			if (prevd.day != dnr.Time.day || prevd.month != dnr.Time.month || prevd.year != dnr.Time.year)
			{
				str.Format("\r\n ===========  %d %s %d  ==========================================\r\n\r\n", dnr.Time.day, AvcGetMonthAbr(dnr.Time.month), dnr.Time.year);
				res += str;
				str.Format("\t<day date=\"%d/%d/%d\" />\n", dnr.Time.day, dnr.Time.month, dnr.Time.year);
				strXml += str;
			}
			prevd = dnr.Time;
		}
		else
		{
			if (prevt != dnr.nType)
			{
				switch(dnr.nType)
				{
				case 10:
				case 11:
				case 12:
				case 13:
					if (prevt < 10 || prevt >= 14)
					{
						res += "\r\n ========== SUNRISE, SUNSET ==========================================\r\n\r\n";
					}
					break;
				case 20:
					res += "\r\n ========== TITHIS ===================================================\r\n\r\n";
					break;
				case 21:
					res += "\r\n ========== NAKSATRAS ================================================\r\n\r\n";
					break;
				case 22:
					res += "\r\n ========== SANKRANTIS ===============================================\r\n\r\n";
					break;
				case 23:
					res += "\r\n ========== SUN-MOON CONJUNCTIONS ====================================\r\n\r\n";
					break;
				}
			}
			prevt = dnr.nType;
		}

		switch(dnr.nType)
		{
		case 10:
			res += "            ";
			res += dnr.Time.c_str();
			res += "   arunodaya\r\n";
			str.Format("\t<event type=\"arunodaya\" time=\"%02d:%02d:%02d\" />\n", dnr.Time.GetHour(), dnr.Time.GetMinute(), dnr.Time.GetSecond());
			strXml += str;
			break;
		case 11:
			res += "            ";
			res += dnr.Time.c_str();
			res += "   sunrise\r\n";
			str.Format("\t<event type=\"sunrise\" time=\"%02d:%02d:%02d\" />\n", dnr.Time.GetHour(), dnr.Time.GetMinute(), dnr.Time.GetSecond());
			strXml += str;
			break;
		case 12:
			res += "            ";
			res += dnr.Time.c_str();
			res += "   noon\r\n";
			str.Format("\t<event type=\"noon\" time=\"%02d:%02d:%02d\" />\n", dnr.Time.GetHour(), dnr.Time.GetMinute(), dnr.Time.GetSecond());
			strXml += str;
			break;
		case 13:
			res += "            ";
			res += dnr.Time.c_str();
			res += "   sunset\r\n";
			str.Format("\t<event type=\"sunset\" time=\"%02d:%02d:%02d\" />\n", dnr.Time.GetHour(), dnr.Time.GetMinute(), dnr.Time.GetSecond());
			strXml += str;
			break;
		case 20:
			res += "            ";
			res += dnr.Time.c_str();
			res += "   ";
			res += GetTithiName(dnr.nData);
			res += " Tithi starts";
			res += "\r\n";
			str.Format("\t<event type=\"%s Tithi\" time=\"%02d:%02d:%02d\" />\n", GetTithiName(dnr.nData)
				, dnr.Time.GetHour(), dnr.Time.GetMinute(), dnr.Time.GetSecond());
			strXml += str;
			break;
		case 21:
			res += "            ";
			res += dnr.Time.c_str();
			res += "   ";
			res += GetNaksatraName(dnr.nData);
			res += " Naksatra starts";
			res += "\r\n";
			str.Format("\t<event type=\"%s Naksatra\" time=\"%02d:%02d:%02d\" />\n", GetNaksatraName(dnr.nData)
				, dnr.Time.GetHour(), dnr.Time.GetMinute(), dnr.Time.GetSecond());
			strXml += str;
			break;
		case 22:
			res += "            ";
			res += dnr.Time.c_str();
			res += "   ";
			res += GetSankrantiName(dnr.nData);
			res += " Sankranti";
			res += "\r\n";
			str.Format("\t<event type=\"%s Sankranti\" time=\"%02d:%02d:%02d\" />\n", GetSankrantiName(dnr.nData)
				, dnr.Time.GetHour(), dnr.Time.GetMinute(), dnr.Time.GetSecond());
			strXml += str;
			break;
		case 23:
			res += "            ";
			res += dnr.Time.c_str();
			res += "   conjunction in ";
			res += GetSankrantiName(dnr.nData);
			res += " rasi";
			res += "\r\n";
			str.Format("\t<event type=\"conjunction in %s rasi\" time=\"%02d:%02d:%02d\" />\n", GetSankrantiName(dnr.nData)
				, dnr.Time.GetHour(), dnr.Time.GetMinute(), dnr.Time.GetSecond());
			strXml += str;
			break;
		default:
			break;
		}
	}

	res += "\r\n";

	strXml += "</xml>\n";

	return 1;
}

void time_print(TString &str, DAYTIME dt)
{
	str.Format("%02d:%02d:%02d", dt.hour, dt.min, dt.sec);
}

void AddListText(TString &text, const char *pText)
{
	TString str;
	str.Format("                 %s\r\n", pText);
	str.TrimRight();
	str+= "\r\n";
	text += str;
}

void AddListText(TString &text, const char *pText, const char * pText2)
{
	TString str;
	str.Format("%-17s%s", pText, pText2);
	str.TrimRight();
	str+= "\r\n";
	text += str;
}

double GetAyanamsa(JULIANDATE);
const char * AvcGetVersionText(void);
extern TString gAyaName;
extern const char * gpszSeparator;


void backprintf(const char * c)
{
	printf("%s", c);
	int n = strlen(c);
	while(n > 0)
	{
		putchar('\b');
		n--;
	}
}

int g_ran_min = 0;
int g_ran_max = 100;

int ProgressWindowCreate()
{
/*	printf("Progress:\n");
	backprintf("                               ");
*/	return 0;
}

int ProgressWindowSetRange(int nMin, int nMax)
{
/*	g_ran_min = nMin;
	g_ran_max = nMax;
*/
	return 0;
}

int ProgressWindowSetPos(int nPos)
{
/*	char sz[32];
	int i = (nPos - g_ran_min)*100/(g_ran_max - g_ran_min);
	for(int a = 0; a < 20; a++)
	{
		sz[a] = (a*5 < i ? '*' : ' ');
	}
	sprintf(sz + 20, "  %3d%% ", i);
	backprintf(sz);*/
	return 0;
}

int ProgressWindowClose()
{
//	printf("                                                                     \n\n");

	return 0;
}

int CalcCalendar(TResultCalendar &daybuff, CLocation & loc, VCTIME date, int nDaysCount) 
{
//	TString str, str2, str3, dayText;

//	char * spaces = "                                                                                ";
//	int lastmasa = -1;
//	int lastmonth = -1;
//	bool bCalcMoon = (GetShowSetVal(4) > 0 || GetShowSetVal(5) > 0);

	if (daybuff.CalculateCalendar(loc, date, nDaysCount) == 0)
		return 0;

	return 1;
}

void FormatCalendarDayHeader(VAISNAVADAY *pd, TString &str)
{
//	static char * dow[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa" };

	TString s1, s2;

	s1 = GetTithiName(pd->astrodata.nTithi);

	if ((pd->astrodata.nTithi == 10) || (pd->astrodata.nTithi == 25) 
		|| (pd->astrodata.nTithi == 11) || (pd->astrodata.nTithi == 26))
	{
		if (pd->ekadasi_parana == false)
		{
			if (pd->nMhdType == EV_NULL)
			{
				s1 += " ";
				s1 += gstr[58];
			}
			else
			{
				s1 += " ";
				s1 += gstr[59];
			}
		}
	}

	gstr[pd->date.dayOfWeek].Left(2, s2);
	str.Format("%2d %s %d %s  %-34s%c %-10s%-15s %s", pd->date.day, AvcGetMonthAbr(pd->date.month), pd->date.year
		, s2.c_str() ,s1.c_str(), GetPaksaChar(pd->astrodata.nPaksa), GetYogaName(pd->astrodata.nYoga)
		, GetNaksatraName(pd->astrodata.nNaksatra), (pd->nFastType == FAST_NULL ? "" : "*"));

}

void FormatCalendarEkadasiParana(VAISNAVADAY * pd, TString &str)
{
	double h1, m1, h2, m2;
	m1 = modf(pd->eparana_time1, &h1);
	if (pd->eparana_time2 >= 0.0)
	{
		m2 = modf(pd->eparana_time2, &h2);
		str.Format("                 %s %02d:%02d - %02d:%02d (%s)", gstr[60].c_str(),
			int(h1), int(m1*60), int(h2), int(m2*60), AvcGetDSTSignature(pd->nDST));
	}
	else if (pd->eparana_time1 >= 0.0)
	{
		str.Format("                 %s %02d:%02d (%s)", gstr[61].c_str(),
			int(h1), int(m1*60), AvcGetDSTSignature(pd->nDST) );
	}
	else
	{
		str = "                 ";
		str += gstr[62];
	}
}

int FormatCalendarDayText(VAISNAVADAY * pvd, TString &dayText) 
{
	TString str, str2, str3;
	int nFestClass;

		dayText.Empty();
		FormatCalendarDayHeader(pvd, str);
		str.Mid(16, str.GetLength(), str2);
		str.Left(15, str3);
		str = str3;
		if (pvd->astrodata.sun.longitude_deg < 0.0)
		{
			AddListText(dayText, str, "No rise and no set of the sun. No calendar information.");
			return 1;
		}
		AddListText(dayText, str, str2);

		if (GetShowSetVal(17) == 1)
		{
			if (pvd->ekadasi_parana)
			{
				FormatCalendarDayHeader(pvd, str);
				str.Delete(0, 17);
				AddListText(dayText, str);
			}
		}

		if (GetShowSetVal(6) == 1)
		{
			if (pvd->festivals.GetLength() > 0)
			{
				int i = pvd->GetHeadFestival();
				while(pvd->GetNextFestival(i, str2))
				{
					if (str2.GetLength() > 1)
					{
						nFestClass = pvd->GetFestivalClass(str2);
						if (nFestClass < 0 || GetShowSetVal(22 + nFestClass) == 1)
							AddListText(dayText, str2);
					}
				}
			}
		}

		if (GetShowSetVal(16) == 1 && pvd->sankranti_zodiac >= 0)
		{
			//double h1, m1;
			//m1 = modf(pvd->sankranti_day.shour*24, &h1);
			str.Format(" %s %s (%s %s %s %d %s, %02d:%02d %s) "
				, GetSankrantiName(pvd->sankranti_zodiac)
				, gstr[56].c_str()
				, gstr[111].c_str(), GetSankrantiNameEn(pvd->sankranti_zodiac)
				, gstr[852].c_str()
				, pvd->sankranti_day.day, AvcGetMonthAbr(pvd->sankranti_day.month)
				, pvd->sankranti_day.GetHour(), pvd->sankranti_day.GetMinuteRound()
				, AvcGetDSTSignature(pvd->nDST));
			int length = str.GetLength();
			int seplen = strlen(gpszSeparator);
			length = (seplen - length) / 2;
			dayText += (gpszSeparator + (seplen - length));
			dayText += str;
			dayText += (gpszSeparator + (seplen - length));
			dayText += "\r\n";
		}

		if (GetShowSetVal(7) == 1 && pvd->was_ksaya)//(m_dshow.m_info_ksaya) && (pvd->was_ksaya))
		{
			double h, m;
			VCTIME ksayaDate;

			// zaciatok ksaya tithi
			m = modf(pvd->ksaya_time1*24, &h);
			ksayaDate = pvd->date;
			if (pvd->ksaya_day1 < 0.0)
				ksayaDate.PreviousDay();
			str2.Format("%d %s, %02d:%02d", ksayaDate.day, AvcGetMonthAbr(ksayaDate.month), int(h), int(m*60));

			// end of ksaya tithi
			m = modf(pvd->ksaya_time2*24, &h);
			ksayaDate = pvd->date;
			if (pvd->ksaya_day2 < 0.0)
				ksayaDate.PreviousDay();
			str3.Format("%d %s, %02d:%02d", ksayaDate.day, AvcGetMonthAbr(ksayaDate.month), int(h), int(m*60));

			// print info
			str.Format("%s %s %s %s %s (%s)", gstr[89].c_str(), gstr[850].c_str(), str2.c_str(), gstr[851].c_str(), str3.c_str(), AvcGetDSTSignature(pvd->nDST));
			AddListText(dayText, str);
		}

		if (GetShowSetVal(8) == 1)//(m_dshow.m_info_vriddhi) && (pvd->is_vriddhi))
		{
			if (pvd->is_vriddhi)
				AddListText(dayText, gstr[90]);
		}

		if (pvd->nCaturmasya & CMASYA_CONT_MASK)
		{
			int n = ((pvd->nCaturmasya & CMASYA_CONT_MASK) >> 22);
			AddListText(dayText, gstr[ 111 + n ]);
		}

		if ((GetShowSetVal(13) == 1) && (pvd->nCaturmasya & CMASYA_PURN_MASK))
		{
			str.Format("%s [PURNIMA SYSTEM]"
				, gstr[107 + (pvd->nCaturmasya & CMASYA_PURN_MASK_DAY)
					       + ((pvd->nCaturmasya & CMASYA_PURN_MASK_MASA) >> 2)].c_str()
				);
			AddListText(dayText, str);
			if ((pvd->nCaturmasya & CMASYA_PURN_MASK_DAY) == 0x1)
			{
				AddListText(dayText, gstr[110 + ((pvd->nCaturmasya & CMASYA_PURN_MASK_MASA) >> 2)].c_str() );
			}
		}

		if ((GetShowSetVal(14) == 1) && (pvd->nCaturmasya & CMASYA_PRAT_MASK))
		{
			str.Format("%s [PRATIPAT SYSTEM]"
				, gstr[107 + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_DAY) >> 8)
					       + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_MASA) >> 10)].c_str()
				);
			AddListText(dayText, str);
			if ((pvd->nCaturmasya & CMASYA_PRAT_MASK_DAY) == 0x100)
			{
				AddListText(dayText, gstr[110 + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_MASA) >> 10)].c_str() );
			}
		}

		if ((GetShowSetVal(15) == 1) && (pvd->nCaturmasya & CMASYA_EKAD_MASK))
		{
			str.Format("%s [EKADASI SYSTEM]"
				, gstr[107 + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_DAY) >> 16)
					       + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_MASA) >> 18)].c_str()
				);
			AddListText(dayText, str);
			if ((pvd->nCaturmasya & CMASYA_EKAD_MASK_DAY) == 0x10000)
			{
				AddListText(dayText, gstr[110 + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_MASA) >> 18)].c_str() );
			}
		}

		// tithi at arunodaya
		if (GetShowSetVal(0) == 1)//m_dshow.m_tithi_arun)
		{
			str.Format("%s: %s", gstr[98].c_str(), GetTithiName(pvd->astrodata.nTithiArunodaya));
			AddListText(dayText, str);
		}

		//"Arunodaya Time",//1
		if (GetShowSetVal(1) == 1)//m_dshow.m_arunodaya)
		{
			str.Format("%s %d:%02d (%s)", gstr[99].c_str(), pvd->astrodata.sun.arunodaya.hour
				, pvd->astrodata.sun.arunodaya.min, AvcGetDSTSignature(pvd->nDST));
			AddListText(dayText, str);
		}
		//"Sunrise Time",//2
		//"Sunset Time",//3
		if (GetShowSetVal(2) == 1)//m_dshow.m_sunrise)
		{
			str.Format("%s %d:%02d (%s)", gstr[51].c_str(), pvd->astrodata.sun.rise.hour
				, pvd->astrodata.sun.rise.min, AvcGetDSTSignature(pvd->nDST));
			AddListText(dayText, str);
		}
		if (GetShowSetVal(3) == 1)//m_dshow.m_sunset)
		{
			str.Format("%s %d:%02d (%s)", gstr[52].c_str(), pvd->astrodata.sun.set.hour
				, pvd->astrodata.sun.set.min, AvcGetDSTSignature(pvd->nDST));
			AddListText(dayText, str);

		}
		//"Moonrise Time",//4
		if (GetShowSetVal(4) == 1)
		{
			if (pvd->moonrise.hour < 0)
				str = gstr[136].c_str();
			else
			{
				str.Format("%s %d:%02d (%s)", gstr[53].c_str(), pvd->moonrise.hour
					, pvd->moonrise.min, AvcGetDSTSignature(pvd->nDST));
			}
			AddListText(dayText, str);
		}
		//"Moonset Time",//5
		if (GetShowSetVal(5) == 1)
		{
			if (pvd->moonrise.hour < 0)
				str = gstr[137];
			else
			{
				str.Format("%s %d:%02d (%s)", gstr[54].c_str(), pvd->moonset.hour
					, pvd->moonset.min, AvcGetDSTSignature(pvd->nDST));
			}
			AddListText(dayText, str);
		}
		///"Sun Longitude",//9
		if (GetShowSetVal(9) == 1)//m_dshow.m_sun_long)
		{
			str.Format("%s: %f (*)", gstr[100].c_str(), pvd->astrodata.sun.longitude_deg);
			AddListText(dayText, str);
		}
		//"Moon Longitude",//10
		if (GetShowSetVal(10) == 1)//m_dshow.m_sun_long)
		{
			str.Format("%s: %f (*)", gstr[101].c_str(), pvd->astrodata.moon.longitude_deg);
			AddListText(dayText, str);
		}
		//"Ayanamsha value",//11
		if (GetShowSetVal(11) == 1)//m_dshow.m_sun_long)
		{
			str.Format("%s %f (%s) (*)", gstr[102].c_str(), pvd->astrodata.msAyanamsa, GetAyanamsaName(GetAyanamsaType()));
			AddListText(dayText, str);
		}
		//"Julian Day",//12
		if (GetShowSetVal(12) == 1)//m_dshow.m_sun_long)
		{
			str.Format("%s %f (*)", gstr[103].c_str(), pvd->astrodata.jdate);
			AddListText(dayText, str);
		}

	return 0;
}

int FormatCalendarOld(TResultCalendar &daybuff, TString &m_text) 
{
	int k;
	TString str, str2, str3, dayText;

	char * spaces = "                                                                                ";
	VAISNAVADAY * pvd, * prevd, *nextd;
	int lastmasa = -1;
	int lastmonth = -1;
	int tp1;
//	double rate;
//	DlgCalcProgress dcp;
//	bool bCalcMoon = (GetShowSetVal(4) > 0 || GetShowSetVal(5) > 0);

	m_text.Empty();

	for (k = 0; k < daybuff.m_vcCount; k++)
	{

		prevd = daybuff.GetDay(k - 1);
		pvd = daybuff.GetDay(k);
		nextd = daybuff.GetDay(k + 1);

		if (pvd)
		{
			if ((GetShowSetVal(18) == 1) && (pvd->astrodata.nMasa != lastmasa))
			{
				m_text += "\r\n";
				str.Format("%s %s, Gaurabda %d", GetMasaName(pvd->astrodata.nMasa), gstr[22].c_str(), pvd->astrodata.nGaurabdaYear);
				tp1 = (80 - str.GetLength())/2;
				str.Insert(0, (spaces + (79 - tp1)));
				str += spaces;
				str.Insert(80 - strlen(AvcGetVersionText()), AvcGetVersionText());
				str.Left(80, str3);
				m_text += str3;
				m_text += "\r\n";
				if ((pvd->astrodata.nMasa == ADHIKA_MASA) && ((lastmasa >= SRIDHARA_MASA) && (lastmasa <= DAMODARA_MASA)))
				{
					AddListText(m_text, gstr[128]);
				}
				m_text += "\r\n";
				m_text += (spaces + (79 - (80 - strlen(daybuff.m_Location.getFullName()))/2));
				m_text += daybuff.m_Location.getFullName();

				m_text += "\r\n\r\n";
				m_text += " DATE            TITHI                         PAKSA YOGA      NAKSATRA       FAST\r\n";
				m_text += gpszSeparator;
				m_text += "\r\n";
				lastmasa = pvd->astrodata.nMasa;
			}

			if ((GetShowSetVal(19) == 1) && (pvd->date.month != lastmonth))
			{
				m_text += "\r\n";
				str.Format("%s %d", gstr[759 + pvd->date.month].c_str(), pvd->date.year);
				tp1 = (80 - str.GetLength())/2;
				str.Insert(0, (spaces + (79 - tp1)));
				str += spaces;
				str.Insert(80 - strlen(AvcGetVersionText()), AvcGetVersionText());
				str.Left(80, str3);
				m_text += str3;
				m_text += "\r\n";
				m_text += (spaces + (79 - (80 - strlen(daybuff.m_Location.getFullName()))/2));
				m_text += daybuff.m_Location.getFullName();

				m_text += "\r\n\r\n";
				m_text += " DATE            TITHI                         PAKSA YOGA      NAKSATRA       FAST\r\n";
				m_text += gpszSeparator;
				m_text += "\r\n";
				lastmonth = pvd->date.month;
			}

			if (FormatCalendarDayText(pvd, dayText) > 0)
				goto _resolve_text;

			if (GetShowSetVal(21) == 1)
			{
				if (prevd != NULL)
				{
					if (prevd->astrodata.nMasa != pvd->astrodata.nMasa)
					{
						str.Format("%s %s %s", gstr[780].c_str(), GetMasaName(pvd->astrodata.nMasa), gstr[22].c_str());
						AddListText(dayText, str);
					}
				}
				if (nextd != NULL)
				{
					if (nextd->astrodata.nMasa != pvd->astrodata.nMasa)
					{
						str.Format("%s %s %s", gstr[781].c_str(), GetMasaName(pvd->astrodata.nMasa), gstr[22].c_str());
						AddListText(dayText, str);
					}
				}
			}

		_resolve_text:
			if (GetShowSetVal(20) == 0)
				m_text += dayText;
			else if (dayText.GetLength() > 90)
				m_text += dayText;


		}
//		date.shour = 0;
//		date.NextDay();
	}

//	dcp.DestroyWindow();

	return 1;
}

class SYSTEMTIME
{
public:
	int wDay;
	int wMonth;
	int wYear;
	int wSecond;
	int wMinute;
	int wHour;
	int wMilliseconds;
	int wDayOfWeek;
};

void GetLocalTime(SYSTEMTIME * st)
{
	time_t t;
	tm * pt;
	time(&t);
	pt = localtime(&t);
	if (pt)
	{
		st->wDay = pt->tm_mday;
		st->wMonth = pt->tm_mon + 1;
		st->wYear = pt->tm_year + 1900;
		st->wHour = pt->tm_hour;
		st->wMinute = pt->tm_min;
		st->wSecond = pt->tm_sec;
		st->wMilliseconds = 0;
		st->wDayOfWeek = pt->tm_wday;
	}
}

int FormatCalendarICAL(TResultCalendar &daybuff, TString &m_text)
{
	int k, nFestClass;
	int initialLength = 0;
	int lastmasa = -1;
	int lastmonth = -1;
	int tzoffset = 0, tzoff;
	TString str, dayText, str2, str3;
	VAISNAVADAY * pvd, * prevd, * nextd;
	char * SPACE_BEFORE_LINE = " , ";
	VCTIME vc;
	CLocation &loc = daybuff.m_Location;

	SYSTEMTIME st;
	GetLocalTime(&st);

	m_text = "BEGIN:VCALENDAR\nVERSION:2.0\nX-WR-CALNAME:VAISNAVA\nPRODID:-//GBC Calendar Comitee//GCAL//EN\n";
	m_text += "X-WR-RELCALID:";
	str2.Format("%08d-%04d-%04d-%04d-%04d%08d", st.wYear + st.wMilliseconds, st.wDay * st.wDayOfWeek, st.wMonth,
		st.wHour, st.wMinute + st.wMilliseconds);
	m_text += str2;
	m_text += "\nX-WR-TIMEZONE:";

	m_text += TTimeZone::GetTimeZoneName(loc.m_nDST);
	m_text += "\n";

	m_text += "CALSCALE:GREGORIAN\nMETHOD:PUBLISH\n";
	m_text += "BEGIN:VTIMEZONE\nTZID:";
	m_text += TTimeZone::GetTimeZoneName(loc.m_nDST);
	str2.Format("\nLAST-MODIFIED:%04d%02d%02dT%02d%02d%02dZ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	m_text += str2;

	tzoffset = int(TTimeZone::GetTimeZoneOffset(loc.m_nDST) * 60.0);
	tzoff = tzoffset + TTimeZone::GetTimeZoneBias(loc.m_nDST);

	if (TTimeZone::GetTimeZoneBias(loc.m_nDST) > 0)
	{
		TTimeZone::GetDaylightTimeStartDate(loc.m_nDST, st.wYear, vc);
		str2.Format("\nBEGIN:DAYLIGHT\nDTSTART:%04d%02d%02dT%02d0000", vc.year, vc.month, vc.day, vc.GetHour());
		m_text += str2;

		str2.Format("\nTZOFFSETTO:%c%02d%02d", (tzoff > 0 ? '+' : '-'), abs(tzoff) / 60, abs(tzoff) % 60);
		m_text += str2;

		str2.Format("\nTZOFFSETFROM:%c%02d%02d", '+', 0, 0);
		m_text += str2;

		TTimeZone::GetNormalTimeStartDate(loc.m_nDST, st.wYear, vc);
		m_text += "\nEND:DAYLIGHT\nBEGIN:STANDARD\nDTSTART:";
		str2.Format("%04d%02d%02dT%02d0000", vc.year, vc.month, vc.day, vc.GetHour());
		m_text += str2;

		str2.Format("\nTZOFFSETTO:%c%02d%02d", (tzoffset > 0 ? '+' : '-'), abs(tzoffset)/60, abs(tzoffset) % 60);
		m_text += str2;
		str2.Format("\nTZOFFSETFROM:%c%02d%02d", (tzoff > 0 ? '+' : '-'), abs(tzoff) / 60, abs(tzoff) % 60);
		m_text += str2;
		m_text += "\nEND:STANDARD\n";
	}
	else
	{
		m_text += "\nBEGIN:STANDARD\nDTSTART:";
		str2.Format("%04d0101T000000", vc.year, vc.month, vc.day, vc.GetHour());
		m_text += str2;

		str.Format("\nTZOFFSETTO:%+02d%02d", tzoffset/60, abs(tzoffset) % 60);
		m_text += str2;
		str2.Format("\nTZOFFSETFROM:+0000");
		m_text += str2;
		m_text += "\nEND:STANDARD\n";
	}

	m_text += "END:VTIMEZONE\n";

	for (k = 0; k < daybuff.m_PureCount; k++)
	{
//		date.shour = 0.0;
//		date.TimeZone = earth.tzone;

		prevd = daybuff.GetDay(k - 1);
		pvd = daybuff.GetDay(k);
		nextd = daybuff.GetDay(k + 1);

		if (pvd)
		{
			dayText.Empty();

			if (pvd->astrodata.nMasa != lastmasa)
			{
				str.Format("%s %s, Gaurabda %d", GetMasaName(pvd->astrodata.nMasa), gstr[22].c_str(), pvd->astrodata.nGaurabdaYear);
				dayText += str;
				dayText += "\n";
				if ((pvd->astrodata.nMasa == ADHIKA_MASA) && ((lastmasa >= SRIDHARA_MASA) && (lastmasa <= DAMODARA_MASA)))
				{
					if (dayText.IsEmpty() == false)
						dayText += SPACE_BEFORE_LINE;
					dayText += gstr[128];
					dayText += "\n";
				}

				lastmasa = pvd->astrodata.nMasa;
				initialLength = -1;
			}
			else
			{
				initialLength = 0;
			}

			if (dayText.IsEmpty() == false)
				dayText += SPACE_BEFORE_LINE;
			dayText += GetTithiName(pvd->astrodata.nTithi);

			if ((pvd->astrodata.nTithi == 10) || (pvd->astrodata.nTithi == 25) || (pvd->astrodata.nTithi == 11) || (pvd->astrodata.nTithi == 26))
			{
				if (pvd->ekadasi_parana == false)
				{
					dayText += " ";
					if (pvd->nMhdType == EV_NULL)
					{
						dayText += gstr[58];
					}
					else
					{
						dayText += gstr[59];
					}
				}
			}
			dayText += "\n";
			initialLength += dayText.GetLength();

			if (pvd->astrodata.sun.longitude_deg < 0.0)
			{
				goto _resolve_text;
			}

//			if (GetShowSetVal(17) == 1)
			{
//				double h1, m1;
				if (pvd->ekadasi_parana)
				{
					FormatCalendarDayHeader(pvd, str);
					str.Delete(0, 17);
					dayText += SPACE_BEFORE_LINE;
					dayText += str;
					dayText += "\n";

/*					m_text += "BEGIN:VEVENT\n";
					if (pvd->eparana_time1 >= 0.0) {
						m1 = modf(pvd->eparana_time1 * 24.0, &h1) * 60;
						str2.Format("DTSTART;TZID=%s:%04d%02d%02dT%02d%02d00\n", TTimeZone::GetTimeZoneName(nDst),
							pvd->date.year, pvd->date.month, pvd->date.day, int(h1), int(m1));
					} else {
						str2.Format("DTSTART;TZID=%s:%04d%02d%02dT%000000\n", TTimeZone::GetTimeZoneName(nDst),
							pvd->date.year, pvd->date.month, pvd->date.day);
					}
					m_text += str2;
					if (pvd->eparana_time2 >= 0.0) {
						m1 = modf(pvd->eparana_time2 * 24.0, &h1) * 60;
						str2.Format("DTEND;TZID=%s:%04d%02d%02dT%02d%02d00\n", TTimeZone::GetTimeZoneName(nDst),
							pvd->date.year, pvd->date.month, pvd->date.day, int(h1), int(m1));
					} else {
						str2.Format("DTEND;TZID=%s:%04d%02d%02dT%235959\n", TTimeZone::GetTimeZoneName(nDst),
							pvd->date.year, pvd->date.month, pvd->date.day);
					}
					m_text += str2;
					m_text += "SUMMARY:";
					m_text += gstr[60];
					m_text += "\nSEQUENCE:1\nEND:VEVENT\n";
*/
				}
			}

//			if (GetShowSetVal(6) == 1)
			{
				if (pvd->festivals)
				{
					int i = pvd->GetHeadFestival();
					while(pvd->GetNextFestival(i, str2))
					{
						if (str2.GetLength() > 1)
						{
							nFestClass = pvd->GetFestivalClass(str2);
							if (nFestClass < 0 || GetShowSetVal(22 + nFestClass) == 1)
							{
								dayText += SPACE_BEFORE_LINE;
								dayText += str2;
								dayText += "\n";
							}
						}
					}
				}
			}

			if (/*GetShowSetVal(16) == 1 &&*/ pvd->sankranti_zodiac >= 0)
			{
				str.Format(" %s %s", GetSankrantiName(pvd->sankranti_zodiac), gstr[56].c_str());
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";
			}

/*			if (GetShowSetVal(7) == 1 && pvd->was_ksaya)//(m_dshow.m_info_ksaya) && (pvd->was_ksaya))
			{
				double h, m;
				VAISNAVADAY * p;


				if (pvd->ksaya_time1 < 0.0)
					m = modf(fabs(1.0 + pvd->ksaya_time1)*24, &h);
				else
					m = modf(pvd->ksaya_time1*24, &h);
				p = (pvd->ksaya_time1 < 0.0) ? prevd : pvd;
				str2.Format("%d %s, %02d:%02d", p->date.day, AvcGetMonthAbr(p->date.month), int(h), int(m*60));

				if (pvd->ksaya_time2 < 0.0)
					m = modf(fabs(1.0 + pvd->ksaya_time2)*24, &h);
				else
					m = modf(pvd->ksaya_time2*24, &h);
				p = (pvd->ksaya_time2 < 0.0) ? prevd : pvd;
				str3.Format("%d %s, %02d:%02d", p->date.day, AvcGetMonthAbr(p->date.month), int(h), int(m*60));

				str.Format("%s %s %s %s %s (%s)", gstr[89], gstr[850], str2, gstr[851], str3, AvcGetDSTSignature(pvd->nDST));
				AddListText(dayText, str);
			}*/

			/*if (GetShowSetVal(8) == 1)//(m_dshow.m_info_vriddhi) && (pvd->is_vriddhi))
			{
				if (pvd->is_vriddhi)
					AddListText(dayText, gstr[90]);
			}*/

			if (pvd->nCaturmasya & CMASYA_CONT_MASK)
			{
				int n = ((pvd->nCaturmasya & CMASYA_CONT_MASK) >> 22);
				dayText += SPACE_BEFORE_LINE;
				dayText += gstr[ 111 + n ].c_str();
				dayText += "\n";
			}

			if ((GetShowSetVal(13) == 1) && (pvd->nCaturmasya & CMASYA_PURN_MASK))
			{
				str.Format("%s [PURNIMA SYSTEM]"
					, gstr[107 + (pvd->nCaturmasya & CMASYA_PURN_MASK_DAY)
					           + ((pvd->nCaturmasya & CMASYA_PURN_MASK_MASA) >> 2)].c_str()
					);
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";
				if ((pvd->nCaturmasya & CMASYA_PURN_MASK_DAY) == 0x1)
				{
					dayText += gstr[110 + ((pvd->nCaturmasya & CMASYA_PURN_MASK_MASA) >> 2)].c_str();
					dayText += "\n";
				}
			}

			if ((GetShowSetVal(14) == 1) && (pvd->nCaturmasya & CMASYA_PRAT_MASK))
			{
				str.Format("%s [PRATIPAT SYSTEM]"
					, gstr[107 + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_DAY) >> 8)
					           + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_MASA) >> 10)].c_str()
					);
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";
				if ((pvd->nCaturmasya & CMASYA_PRAT_MASK_DAY) == 0x100)
				{
					dayText += SPACE_BEFORE_LINE;
					dayText += gstr[110 + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_MASA) >> 10)].c_str();
					dayText += "\n";
				}
			}

			if ((GetShowSetVal(15) == 1) && (pvd->nCaturmasya & CMASYA_EKAD_MASK))
			{
				str.Format("%s [EKADASI SYSTEM]"
					, gstr[107 + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_DAY) >> 16)
					           + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_MASA) >> 18)].c_str()
					);
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";
				if ((pvd->nCaturmasya & CMASYA_EKAD_MASK_DAY) == 0x10000)
				{
					dayText += SPACE_BEFORE_LINE;
					dayText += gstr[110 + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_MASA) >> 18)].c_str();
					dayText += "\n";
				}
			}

/*			// tithi at arunodaya
			if (GetShowSetVal(0) == 1)//m_dshow.m_tithi_arun)
			{
				str.Format("%s: %s", gstr[98], GetTithiName(pvd->astrodata.nTithiArunodaya));
				AddListText(dayText, str);
			}

			//"Arunodaya Time",//1
			if (GetShowSetVal(1) == 1)//m_dshow.m_arunodaya)
			{
				str.Format("%s %d:%02d (%s)", gstr[99], pvd->astrodata.sun.arunodaya.hour
					, pvd->astrodata.sun.arunodaya.min, AvcGetDSTSignature(pvd->nDST));
				AddListText(dayText, str);
			}
*/			//"Sunrise Time",//2
			//"Sunset Time",//3
			if (GetShowSetVal(2) == 1)//m_dshow.m_sunrise)
			{
				str.Format("%s %d:%02d (%s)", gstr[51].c_str(), pvd->astrodata.sun.rise.hour
					, pvd->astrodata.sun.rise.min, AvcGetDSTSignature(pvd->nDST));
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";
			}
			if (GetShowSetVal(3) == 1)//m_dshow.m_sunset)
			{
				str.Format("%s %d:%02d (%s)", gstr[52].c_str(), pvd->astrodata.sun.set.hour
					, pvd->astrodata.sun.set.min, AvcGetDSTSignature(pvd->nDST));
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";

			}
/*			//"Moonrise Time",//4
			if (GetShowSetVal(4) == 1)
			{
				if (pvd->moonrise.hour < 0)
					str = gstr[136];
				else
				{
					str.Format("%s %d:%02d (%s)", gstr[53], pvd->moonrise.hour
						, pvd->moonrise.min, AvcGetDSTSignature(pvd->nDST));
				}
				AddListText(dayText, str);
			}
			//"Moonset Time",//5
			if (GetShowSetVal(5) == 1)
			{
				if (pvd->moonrise.hour < 0)
					str = gstr[137];
				else
				{
					str.Format("%s %d:%02d (%s)", gstr[54], pvd->moonset.hour
						, pvd->moonset.min, AvcGetDSTSignature(pvd->nDST));
				}
				AddListText(dayText, str);
			}
			///"Sun Longitude",//9
			if (GetShowSetVal(9) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s: %f", gstr[100], pvd->astrodata.sun.longitude_deg);
				AddListText(dayText, str);
			}
			//"Moon Longitude",//10
			if (GetShowSetVal(10) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s: %f", gstr[101], pvd->astrodata.moon.longitude_deg);
				AddListText(dayText, str);
			}
			//"Ayanamsha value",//11
			if (GetShowSetVal(11) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s %f (%s)", gstr[102], pvd->astrodata.msAyanamsa, GetAyanamsaName(GetAyanamsaType()));
				AddListText(dayText, str);
			}
			//"Julian Day",//12
			if (GetShowSetVal(12) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s %f", gstr[103], pvd->astrodata.jdate);
				AddListText(dayText, str);
			}

*///			if (GetShowSetVal(21) == 1)
			{
				if (prevd != NULL)
				{
					if (prevd->astrodata.nMasa != pvd->astrodata.nMasa)
					{
						str.Format("%s %s %s", gstr[780].c_str(), GetMasaName(pvd->astrodata.nMasa), gstr[22].c_str());
						dayText += SPACE_BEFORE_LINE;
						dayText += str;
						dayText += "\n";
					}
				}
				if (nextd != NULL)
				{
					if (nextd->astrodata.nMasa != pvd->astrodata.nMasa)
					{
						str.Format("%s %s %s", gstr[781].c_str(), GetMasaName(pvd->astrodata.nMasa), gstr[22].c_str());
						dayText += SPACE_BEFORE_LINE;
						dayText += str;
						dayText += "\n";
					}
				}
			}

		_resolve_text:
			if (dayText.GetLength() > initialLength)
			{
				m_text += "BEGIN:VEVENT\n";
				str2.Format("DTSTART;VALUE=DATE:%04d%02d%02d\n", pvd->date.year, pvd->date.month, pvd->date.day);
				m_text += str2;
				/*str2.Format("DTEND;VALUE=DATE:%04d%02d%02dT%02d%02d%02d\n", pvd->date.year, pvd->date.month, pvd->date.day,
					pvd->astrodata.sun.set.hour, pvd->astrodata.sun.set.min, pvd->astrodata.sun.set.sec);
				m_text += str2;*/
				str2.Format("LOCATION:%s\n", loc.m_strCity.c_str());
				str2.Replace(",", "\\,");
				m_text += str2;
				m_text += "SUMMARY:";
				dayText.TrimLeft();
				dayText.Replace(",", "\\,");
				m_text += dayText;
				str2.Format("UID:%08d-%04d-%04d-%04d-%08d%04d\n", st.wYear, st.wMonth*30 + st.wDay, st.wHour*60 + st.wMinute, st.wSecond, st.wMilliseconds, k);
				m_text += str2;
				m_text += "DURATION:P1D\nSEQUENCE:1\nEND:VEVENT\n";
			}
		}
	}

	m_text += "END:VCALENDAR\n";
	return 1;
}

int FormatCalendarCSV(TResultCalendar &daybuff, TString &m_text)
{
	int k, nFestClass;
	int initialLength = 0;
	int lastmasa = -1;
	int lastmonth = -1;
	int tzoffset = 0;
	TString str, dayText, str2, str3;
	VAISNAVADAY * pvd, * prevd, * nextd;
	char * SPACE_BEFORE_LINE = " , ";
	VCTIME vc;

	SYSTEMTIME st;
	GetLocalTime(&st);

	m_text = "\"Subject\",\"Begin Date\",\"Start\",\"End Date\",\"End\",\"WholeDay\",\"Alarm\"\n";

	for (k = 0; k < daybuff.m_PureCount; k++)
	{
//		date.shour = 0.0;
//		date.TimeZone = earth.tzone;

		prevd = daybuff.GetDay(k - 1);
		pvd = daybuff.GetDay(k);
		nextd = daybuff.GetDay(k + 1);

		if (pvd)
		{
			dayText.Empty();

			if (pvd->astrodata.nMasa != lastmasa)
			{
				lastmasa = pvd->astrodata.nMasa;
				initialLength = -1;
			}
			else
			{
				initialLength = 0;
			}

			if (dayText.IsEmpty() == false)
				dayText += SPACE_BEFORE_LINE;
			dayText += GetTithiName(pvd->astrodata.nTithi);

			if ((pvd->astrodata.nTithi == 10) || (pvd->astrodata.nTithi == 25) || (pvd->astrodata.nTithi == 11) || (pvd->astrodata.nTithi == 26))
			{
				if (pvd->ekadasi_parana == false)
				{
					dayText += " ";
					if (pvd->nMhdType == EV_NULL)
					{
						dayText += gstr[58];
					}
					else
					{
						dayText += gstr[59];
					}
				}
			}
			dayText += "; ";
			initialLength += dayText.GetLength();

			if (pvd->astrodata.sun.longitude_deg < 0.0)
			{
				goto _resolve_text;
			}

//			if (GetShowSetVal(17) == 1)
			{
//				double h1, m1;
				if (pvd->ekadasi_parana)
				{
					FormatCalendarDayHeader(pvd, str);
					str.Delete(0, 17);
					dayText += str;
					dayText += "; ";

/*					m_text += "BEGIN:VEVENT\n";
					if (pvd->eparana_time1 >= 0.0) {
						m1 = modf(pvd->eparana_time1 * 24.0, &h1) * 60;
						str2.Format("DTSTART;TZID=%s:%04d%02d%02dT%02d%02d00\n", TTimeZone::GetTimeZoneName(nDst),
							pvd->date.year, pvd->date.month, pvd->date.day, int(h1), int(m1));
					} else {
						str2.Format("DTSTART;TZID=%s:%04d%02d%02dT%000000\n", TTimeZone::GetTimeZoneName(nDst),
							pvd->date.year, pvd->date.month, pvd->date.day);
					}
					m_text += str2;
					if (pvd->eparana_time2 >= 0.0) {
						m1 = modf(pvd->eparana_time2 * 24.0, &h1) * 60;
						str2.Format("DTEND;TZID=%s:%04d%02d%02dT%02d%02d00\n", TTimeZone::GetTimeZoneName(nDst),
							pvd->date.year, pvd->date.month, pvd->date.day, int(h1), int(m1));
					} else {
						str2.Format("DTEND;TZID=%s:%04d%02d%02dT%235959\n", TTimeZone::GetTimeZoneName(nDst),
							pvd->date.year, pvd->date.month, pvd->date.day);
					}
					m_text += str2;
					m_text += "SUMMARY:";
					m_text += gstr[60];
					m_text += "\nSEQUENCE:1\nEND:VEVENT\n";
*/
				}
			}

//			if (GetShowSetVal(6) == 1)
			{
				if (pvd->festivals)
				{
					int i = pvd->GetHeadFestival();
					while(pvd->GetNextFestival(i, str2))
					{
						if (str2.GetLength() > 1)
						{
							nFestClass = pvd->GetFestivalClass(str2);
							if (nFestClass < 0 || GetShowSetVal(22 + nFestClass) == 1)
							{
								dayText += str2;
								dayText += "; ";
							}
						}
					}
				}
			}

			if (/*GetShowSetVal(16) == 1 &&*/ pvd->sankranti_zodiac >= 0)
			{
				str.Format("%s %s; ", GetSankrantiName(pvd->sankranti_zodiac), gstr[56].c_str());
				dayText += str;
			}

/*			if (GetShowSetVal(7) == 1 && pvd->was_ksaya)//(m_dshow.m_info_ksaya) && (pvd->was_ksaya))
			{
				double h, m;
				VAISNAVADAY * p;


				if (pvd->ksaya_time1 < 0.0)
					m = modf(fabs(1.0 + pvd->ksaya_time1)*24, &h);
				else
					m = modf(pvd->ksaya_time1*24, &h);
				p = (pvd->ksaya_time1 < 0.0) ? prevd : pvd;
				str2.Format("%d %s, %02d:%02d", p->date.day, AvcGetMonthAbr(p->date.month), int(h), int(m*60));

				if (pvd->ksaya_time2 < 0.0)
					m = modf(fabs(1.0 + pvd->ksaya_time2)*24, &h);
				else
					m = modf(pvd->ksaya_time2*24, &h);
				p = (pvd->ksaya_time2 < 0.0) ? prevd : pvd;
				str3.Format("%d %s, %02d:%02d", p->date.day, AvcGetMonthAbr(p->date.month), int(h), int(m*60));

				str.Format("%s %s %s %s %s (%s)", gstr[89], gstr[850], str2, gstr[851], str3, AvcGetDSTSignature(pvd->nDST));
				AddListText(dayText, str);
			}*/

			/*if (GetShowSetVal(8) == 1)//(m_dshow.m_info_vriddhi) && (pvd->is_vriddhi))
			{
				if (pvd->is_vriddhi)
					AddListText(dayText, gstr[90]);
			}*/

			if (pvd->nCaturmasya & CMASYA_CONT_MASK)
			{
				int n = ((pvd->nCaturmasya & CMASYA_CONT_MASK) >> 22);
				dayText += gstr[ 111 + n ];
				dayText += "; ";
			}

			if ((GetShowSetVal(13) == 1) && (pvd->nCaturmasya & CMASYA_PURN_MASK))
			{
				str.Format("%s [PURNIMA SYSTEM]; "
					, gstr[107 + (pvd->nCaturmasya & CMASYA_PURN_MASK_DAY)
					           + ((pvd->nCaturmasya & CMASYA_PURN_MASK_MASA) >> 2)].c_str()
					);
				dayText += str;
				if ((pvd->nCaturmasya & CMASYA_PURN_MASK_DAY) == 0x1)
				{
					dayText += gstr[110 + ((pvd->nCaturmasya & CMASYA_PURN_MASK_MASA) >> 2)].c_str();
					dayText += "; ";
				}
			}

			if ((GetShowSetVal(14) == 1) && (pvd->nCaturmasya & CMASYA_PRAT_MASK))
			{
				str.Format("%s [PRATIPAT SYSTEM]; "
					, gstr[107 + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_DAY) >> 8)
					           + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_MASA) >> 10)].c_str()
					);
				dayText += str;
				if ((pvd->nCaturmasya & CMASYA_PRAT_MASK_DAY) == 0x100)
				{
					dayText += gstr[110 + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_MASA) >> 10)].c_str();
					dayText += "; ";
				}
			}

			if ((GetShowSetVal(15) == 1) && (pvd->nCaturmasya & CMASYA_EKAD_MASK))
			{
				str.Format("%s [EKADASI SYSTEM]; "
					, gstr[107 + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_DAY) >> 16)
					           + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_MASA) >> 18)].c_str()
					);
				dayText += str;
				if ((pvd->nCaturmasya & CMASYA_EKAD_MASK_DAY) == 0x10000)
				{
					dayText += gstr[110 + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_MASA) >> 18)].c_str();
					dayText += "; ";
				}
			}

/*			// tithi at arunodaya
			if (GetShowSetVal(0) == 1)//m_dshow.m_tithi_arun)
			{
				str.Format("%s: %s", gstr[98], GetTithiName(pvd->astrodata.nTithiArunodaya));
				AddListText(dayText, str);
			}

			//"Arunodaya Time",//1
			if (GetShowSetVal(1) == 1)//m_dshow.m_arunodaya)
			{
				str.Format("%s %d:%02d (%s)", gstr[99], pvd->astrodata.sun.arunodaya.hour
					, pvd->astrodata.sun.arunodaya.min, AvcGetDSTSignature(pvd->nDST));
				AddListText(dayText, str);
			}
*/			//"Sunrise Time",//2
			//"Sunset Time",//3
			//if (GetShowSetVal(2) == 1)//m_dshow.m_sunrise)
			{
				str.Format("Sun %d:%02d - ", pvd->astrodata.sun.rise.hour
					, pvd->astrodata.sun.rise.min);
				dayText += str;
			}
			//if (GetShowSetVal(3) == 1)//m_dshow.m_sunset)
			{
				str.Format("%d:%02d", pvd->astrodata.sun.set.hour, pvd->astrodata.sun.set.min);
				dayText += str;
				dayText += "; ";

			}
/*			//"Moonrise Time",//4
			if (GetShowSetVal(4) == 1)
			{
				if (pvd->moonrise.hour < 0)
					str = gstr[136];
				else
				{
					str.Format("%s %d:%02d (%s)", gstr[53], pvd->moonrise.hour
						, pvd->moonrise.min, AvcGetDSTSignature(pvd->nDST));
				}
				AddListText(dayText, str);
			}
			//"Moonset Time",//5
			if (GetShowSetVal(5) == 1)
			{
				if (pvd->moonrise.hour < 0)
					str = gstr[137];
				else
				{
					str.Format("%s %d:%02d (%s)", gstr[54], pvd->moonset.hour
						, pvd->moonset.min, AvcGetDSTSignature(pvd->nDST));
				}
				AddListText(dayText, str);
			}
			///"Sun Longitude",//9
			if (GetShowSetVal(9) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s: %f", gstr[100], pvd->astrodata.sun.longitude_deg);
				AddListText(dayText, str);
			}
			//"Moon Longitude",//10
			if (GetShowSetVal(10) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s: %f", gstr[101], pvd->astrodata.moon.longitude_deg);
				AddListText(dayText, str);
			}
			//"Ayanamsha value",//11
			if (GetShowSetVal(11) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s %f (%s)", gstr[102], pvd->astrodata.msAyanamsa, GetAyanamsaName(GetAyanamsaType()));
				AddListText(dayText, str);
			}
			//"Julian Day",//12
			if (GetShowSetVal(12) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s %f", gstr[103], pvd->astrodata.jdate);
				AddListText(dayText, str);
			}

*///			if (GetShowSetVal(21) == 1)
			{
				if (prevd != NULL)
				{
					if (prevd->astrodata.nMasa != pvd->astrodata.nMasa)
					{
						str.Format("%s %s %s", gstr[780].c_str(), GetMasaName(pvd->astrodata.nMasa), gstr[22].c_str());
						dayText += str;
						dayText += "; ";
					}
				}
				if (nextd != NULL)
				{
					if (nextd->astrodata.nMasa != pvd->astrodata.nMasa)
					{
						str.Format("%s %s %s", gstr[781].c_str(), GetMasaName(pvd->astrodata.nMasa), gstr[22].c_str());
						dayText += str;
						dayText += "; ";
					}
				}
			}

		_resolve_text:
			if (dayText.GetLength() > initialLength)
			{
				m_text += "\"";
				m_text += dayText;
				m_text += "\",";

				str2.Format("\"%d.%d.%d\",\"0:00:00\",\"%d.%d.%d\",\"0:00:00\",\"True\",\"False\"\n", 
					pvd->date.day, pvd->date.month, pvd->date.year, nextd->date.day,
					nextd->date.month, nextd->date.year);
				m_text += str2;
			}
		}
	}

	return 1;
}

int FormatCalendarVCAL(TResultCalendar &daybuff, TString &m_text)
{
	int k, nFestClass;
	int initialLength = 0;
	int lastmasa = -1;
	int lastmonth = -1;
	int tzoffset = 0;
	TString str, dayText, str2, str3;
	VAISNAVADAY * pvd, * prevd, * nextd;
	const char * SPACE_BEFORE_LINE = " , ";
	VCTIME vc;

	SYSTEMTIME st;
	GetLocalTime(&st);

	m_text = "BEGIN:VCALENDAR\nVERSION:1.0\nX-WR-CALNAME:VAISNAVA\nPRODID:-//GBC Calendar Comitee//GCAL//EN\n";
	m_text += "X-WR-RELCALID:";
	str2.Format("%08d-%04d-%04d-%04d-%04d%08d", st.wYear + st.wMilliseconds, st.wDay * st.wDayOfWeek, st.wMonth,
		st.wHour, st.wMinute + st.wMilliseconds);
	m_text += str2;
	m_text += "\nX-WR-TIMEZONE:";

	m_text += TTimeZone::GetTimeZoneName(daybuff.m_Location.m_nDST);
	m_text += "\n";

	m_text += "CALSCALE:GREGORIAN\nMETHOD:PUBLISH\n";
	/*m_text += "BEGIN:VTIMEZONE\nTZID:";
	m_text += TTimeZone::GetTimeZoneName(nDst);
	str2.Format("\nLAST-MODIFIED:%04d%02d%02dT%02d%02d%02dZ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	m_text += str2;

	tzoffset = int(TTimeZone::GetTimeZoneOffset(nDst) * 60.0);
	tzoff = tzoffset + TTimeZone::GetTimeZoneBias(nDst);

	if (TTimeZone::GetTimeZoneBias(nDst) > 0)
	{
		TTimeZone::GetDaylightTimeStartDate(nDst, st.wYear, vc);
		str2.Format("\nBEGIN:DAYLIGHT\nDTSTART:%04d%02d%02dT%02d0000", vc.year, vc.month, vc.day, vc.GetHour());
		m_text += str2;

		str2.Format("\nTZOFFSETTO:%c%02d%02d", (tzoff > 0 ? '+' : '-'), abs(tzoff) / 60, abs(tzoff) % 60);
		m_text += str2;

		str2.Format("\nTZOFFSETFROM:%c%02d%02d", '+', 0, 0);
		m_text += str2;

		TTimeZone::GetNormalTimeStartDate(nDst, st.wYear, vc);
		m_text += "\nEND:DAYLIGHT\nBEGIN:STANDARD\nDTSTART:";
		str2.Format("%04d%02d%02dT%02d0000", vc.year, vc.month, vc.day, vc.GetHour());
		m_text += str2;

		str2.Format("\nTZOFFSETTO:%c%02d%02d", (tzoffset > 0 ? '+' : '-'), abs(tzoffset)/60, abs(tzoffset) % 60);
		m_text += str2;
		str2.Format("\nTZOFFSETFROM:%c%02d%02d", (tzoff > 0 ? '+' : '-'), abs(tzoff) / 60, abs(tzoff) % 60);
		m_text += str2;
		m_text += "\nEND:STANDARD\n";
	}
	else
	{
		m_text += "\nBEGIN:STANDARD\nDTSTART:";
		str2.Format("%04d0101T000000", vc.year, vc.month, vc.day, vc.GetHour());
		m_text += str2;

		str.Format("\nTZOFFSETTO:%+02d%02d", tzoffset/60, abs(tzoffset) % 60);
		m_text += str2;
		str2.Format("\nTZOFFSETFROM:+0000");
		m_text += str2;
		m_text += "\nEND:STANDARD\n";
	}

	m_text += "END:VTIMEZONE\n";
*/
	for (k = 0; k < daybuff.m_PureCount; k++)
	{
//		date.shour = 0.0;
//		date.TimeZone = earth.tzone;

		prevd = daybuff.GetDay(k - 1);
		pvd = daybuff.GetDay(k);
		nextd = daybuff.GetDay(k + 1);

		if (pvd)
		{
			dayText.Empty();

			if (pvd->astrodata.nMasa != lastmasa)
			{
				str.Format("%s %s, Gaurabda %d", GetMasaName(pvd->astrodata.nMasa), gstr[22].c_str(), pvd->astrodata.nGaurabdaYear);
				dayText += str;
				dayText += "\n";
				if ((pvd->astrodata.nMasa == ADHIKA_MASA) && ((lastmasa >= SRIDHARA_MASA) && (lastmasa <= DAMODARA_MASA)))
				{
					if (dayText.IsEmpty() == false)
						dayText += SPACE_BEFORE_LINE;
					dayText += gstr[128];
					dayText += "\n";
				}

				lastmasa = pvd->astrodata.nMasa;
				initialLength = -1;
			}
			else
			{
				initialLength = 0;
			}

			if (dayText.IsEmpty() == false)
				dayText += SPACE_BEFORE_LINE;
			dayText += GetTithiName(pvd->astrodata.nTithi);

			if ((pvd->astrodata.nTithi == 10) || (pvd->astrodata.nTithi == 25) || (pvd->astrodata.nTithi == 11) || (pvd->astrodata.nTithi == 26))
			{
				if (pvd->ekadasi_parana == false)
				{
					dayText += " ";
					if (pvd->nMhdType == EV_NULL)
					{
						dayText += gstr[58];
					}
					else
					{
						dayText += gstr[59];
					}
				}
			}
			dayText += "\n";
			initialLength += dayText.GetLength();

			if (pvd->astrodata.sun.longitude_deg < 0.0)
			{
				goto _resolve_text;
			}

//			if (GetShowSetVal(17) == 1)
			{
				double h1, m1;
				if (pvd->ekadasi_parana)
				{
/*					pvd->GetTextEP(str);
					str.Delete(0, 17);
					dayText += SPACE_BEFORE_LINE;
					dayText += str;
					dayText += "\n";
*/
					m_text += "BEGIN:VEVENT\n";
					if (pvd->eparana_time1 >= 0.0) {
						m1 = modf(pvd->eparana_time1, &h1) * 60;
						str2.Format("DTSTART:%04d%02d%02dT%02d%02d00\n", pvd->date.year, pvd->date.month, pvd->date.day, int(h1), int(m1));
					} else {
						str2.Format("DTSTART:%04d%02d%02dT000000\n", pvd->date.year, pvd->date.month, pvd->date.day);
					}
					m_text += str2;
					if (pvd->eparana_time2 >= 0.0) {
						m1 = modf(pvd->eparana_time2, &h1) * 60;
						str2.Format("DTEND:%04d%02d%02dT%02d%02d00\n", pvd->date.year, pvd->date.month, pvd->date.day, int(h1), int(m1));
					} else {
						str2.Format("DTEND:%04d%02d%02dT%02d%02d%00\n", pvd->date.year, pvd->date.month, pvd->date.day, pvd->astrodata.sun.set.hour, pvd->astrodata.sun.set.min);
					}
					m_text += str2;
					m_text += "SUMMARY:";
					m_text += gstr[60];
					m_text += "\nSEQUENCE:1\nEND:VEVENT\n";

				}
			}

//			if (GetShowSetVal(6) == 1)
			{
				if (pvd->festivals)
				{
					int i = pvd->GetHeadFestival();
					while(pvd->GetNextFestival(i, str2))
					{
						if (str2.GetLength() > 1)
						{
							nFestClass = pvd->GetFestivalClass(str2);
							if (nFestClass < 0 || GetShowSetVal(22 + nFestClass) == 1)
							{
								dayText += SPACE_BEFORE_LINE;
								dayText += str2;
								dayText += "\n";
							}
						}
					}
				}
			}

			if (/*GetShowSetVal(16) == 1 &&*/ pvd->sankranti_zodiac >= 0)
			{
				str.Format(" %s %s", GetSankrantiName(pvd->sankranti_zodiac), gstr[56].c_str());
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";
			}

/*			if (GetShowSetVal(7) == 1 && pvd->was_ksaya)//(m_dshow.m_info_ksaya) && (pvd->was_ksaya))
			{
				double h, m;
				VAISNAVADAY * p;


				if (pvd->ksaya_time1 < 0.0)
					m = modf(fabs(1.0 + pvd->ksaya_time1)*24, &h);
				else
					m = modf(pvd->ksaya_time1*24, &h);
				p = (pvd->ksaya_time1 < 0.0) ? prevd : pvd;
				str2.Format("%d %s, %02d:%02d", p->date.day, AvcGetMonthAbr(p->date.month), int(h), int(m*60));

				if (pvd->ksaya_time2 < 0.0)
					m = modf(fabs(1.0 + pvd->ksaya_time2)*24, &h);
				else
					m = modf(pvd->ksaya_time2*24, &h);
				p = (pvd->ksaya_time2 < 0.0) ? prevd : pvd;
				str3.Format("%d %s, %02d:%02d", p->date.day, AvcGetMonthAbr(p->date.month), int(h), int(m*60));

				str.Format("%s %s %s %s %s (%s)", gstr[89], gstr[850], str2, gstr[851], str3, AvcGetDSTSignature(pvd->nDST));
				AddListText(dayText, str);
			}*/

			/*if (GetShowSetVal(8) == 1)//(m_dshow.m_info_vriddhi) && (pvd->is_vriddhi))
			{
				if (pvd->is_vriddhi)
					AddListText(dayText, gstr[90]);
			}*/

			if (pvd->nCaturmasya & CMASYA_CONT_MASK)
			{
				int n = ((pvd->nCaturmasya & CMASYA_CONT_MASK) >> 22);
				dayText += SPACE_BEFORE_LINE;
				dayText += gstr[ 111 + n ];
				dayText += "\n";
			}

			if ((GetShowSetVal(13) == 1) && (pvd->nCaturmasya & CMASYA_PURN_MASK))
			{
				str.Format("%s [PURNIMA SYSTEM]"
					, gstr[107 + (pvd->nCaturmasya & CMASYA_PURN_MASK_DAY)
					           + ((pvd->nCaturmasya & CMASYA_PURN_MASK_MASA) >> 2)].c_str()
					);
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";
				if ((pvd->nCaturmasya & CMASYA_PURN_MASK_DAY) == 0x1)
				{
					dayText += gstr[110 + ((pvd->nCaturmasya & CMASYA_PURN_MASK_MASA) >> 2)].c_str();
					dayText += "\n";
				}
			}

			if ((GetShowSetVal(14) == 1) && (pvd->nCaturmasya & CMASYA_PRAT_MASK))
			{
				str.Format("%s [PRATIPAT SYSTEM]"
					, gstr[107 + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_DAY) >> 8)
					           + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_MASA) >> 10)].c_str()
					);
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";
				if ((pvd->nCaturmasya & CMASYA_PRAT_MASK_DAY) == 0x100)
				{
					dayText += SPACE_BEFORE_LINE;
					dayText += gstr[110 + ((pvd->nCaturmasya & CMASYA_PRAT_MASK_MASA) >> 10)].c_str();
					dayText += "\n";
				}
			}

			if ((GetShowSetVal(15) == 1) && (pvd->nCaturmasya & CMASYA_EKAD_MASK))
			{
				str.Format("%s [EKADASI SYSTEM]"
					, gstr[107 + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_DAY) >> 16)
					           + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_MASA) >> 18)].c_str()
					);
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";
				if ((pvd->nCaturmasya & CMASYA_EKAD_MASK_DAY) == 0x10000)
				{
					dayText += SPACE_BEFORE_LINE;
					dayText += gstr[110 + ((pvd->nCaturmasya & CMASYA_EKAD_MASK_MASA) >> 18)].c_str();
					dayText += "\n";
				}
			}

/*			// tithi at arunodaya
			if (GetShowSetVal(0) == 1)//m_dshow.m_tithi_arun)
			{
				str.Format("%s: %s", gstr[98], GetTithiName(pvd->astrodata.nTithiArunodaya));
				AddListText(dayText, str);
			}

			//"Arunodaya Time",//1
			if (GetShowSetVal(1) == 1)//m_dshow.m_arunodaya)
			{
				str.Format("%s %d:%02d (%s)", gstr[99], pvd->astrodata.sun.arunodaya.hour
					, pvd->astrodata.sun.arunodaya.min, AvcGetDSTSignature(pvd->nDST));
				AddListText(dayText, str);
			}
*/			//"Sunrise Time",//2
			//"Sunset Time",//3
			if (GetShowSetVal(2) == 1)//m_dshow.m_sunrise)
			{
				str.Format("%s %d:%02d (%s)", gstr[51].c_str(), pvd->astrodata.sun.rise.hour
					, pvd->astrodata.sun.rise.min, AvcGetDSTSignature(pvd->nDST));
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";
			}
			if (GetShowSetVal(3) == 1)//m_dshow.m_sunset)
			{
				str.Format("%s %d:%02d (%s)", gstr[52].c_str(), pvd->astrodata.sun.set.hour
					, pvd->astrodata.sun.set.min, AvcGetDSTSignature(pvd->nDST));
				dayText += SPACE_BEFORE_LINE;
				dayText += str;
				dayText += "\n";

			}
/*			//"Moonrise Time",//4
			if (GetShowSetVal(4) == 1)
			{
				if (pvd->moonrise.hour < 0)
					str = gstr[136];
				else
				{
					str.Format("%s %d:%02d (%s)", gstr[53], pvd->moonrise.hour
						, pvd->moonrise.min, AvcGetDSTSignature(pvd->nDST));
				}
				AddListText(dayText, str);
			}
			//"Moonset Time",//5
			if (GetShowSetVal(5) == 1)
			{
				if (pvd->moonrise.hour < 0)
					str = gstr[137];
				else
				{
					str.Format("%s %d:%02d (%s)", gstr[54], pvd->moonset.hour
						, pvd->moonset.min, AvcGetDSTSignature(pvd->nDST));
				}
				AddListText(dayText, str);
			}
			///"Sun Longitude",//9
			if (GetShowSetVal(9) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s: %f", gstr[100], pvd->astrodata.sun.longitude_deg);
				AddListText(dayText, str);
			}
			//"Moon Longitude",//10
			if (GetShowSetVal(10) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s: %f", gstr[101], pvd->astrodata.moon.longitude_deg);
				AddListText(dayText, str);
			}
			//"Ayanamsha value",//11
			if (GetShowSetVal(11) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s %f (%s)", gstr[102], pvd->astrodata.msAyanamsa, GetAyanamsaName(GetAyanamsaType()));
				AddListText(dayText, str);
			}
			//"Julian Day",//12
			if (GetShowSetVal(12) == 1)//m_dshow.m_sun_long)
			{
				str.Format("%s %f", gstr[103], pvd->astrodata.jdate);
				AddListText(dayText, str);
			}

*///			if (GetShowSetVal(21) == 1)
			{
				if (prevd != NULL)
				{
					if (prevd->astrodata.nMasa != pvd->astrodata.nMasa)
					{
						str.Format("%s %s %s", gstr[780].c_str(), GetMasaName(pvd->astrodata.nMasa), gstr[22].c_str());
						dayText += SPACE_BEFORE_LINE;
						dayText += str;
						dayText += "\n";
					}
				}
				if (nextd != NULL)
				{
					if (nextd->astrodata.nMasa != pvd->astrodata.nMasa)
					{
						str.Format("%s %s %s", gstr[781].c_str(), GetMasaName(pvd->astrodata.nMasa), gstr[22].c_str());
						dayText += SPACE_BEFORE_LINE;
						dayText += str;
						dayText += "\n";
					}
				}
			}

		_resolve_text:
			if (dayText.GetLength() > initialLength)
			{
				m_text += "BEGIN:VEVENT\n";
				str2.Format("DTSTART:%04d%02d%02dT%02d%02d%02d\n", pvd->date.year, pvd->date.month, pvd->date.day,
					pvd->astrodata.sun.rise.hour, pvd->astrodata.sun.rise.min, pvd->astrodata.sun.rise.sec);
				m_text += str2;
				str2.Format("DTEND:%04d%02d%02dT%02d%02d%02d\n", pvd->date.year, pvd->date.month, pvd->date.day,
					pvd->astrodata.sun.set.hour, pvd->astrodata.sun.set.min, pvd->astrodata.sun.set.sec);
				m_text += str2;
				str2.Format("LOCATION:%s\n", daybuff.m_Location.m_strCity.c_str());
				str2.Replace(",", "\\,");
				m_text += str2;
				m_text += "SUMMARY:";
				dayText.TrimLeft();
				dayText.Replace(",", "\\,");
				m_text += dayText;
				str2.Format("UID:%08d-%04d-%04d-%04d-%08d%04d\n", st.wYear, st.wMonth*30 + st.wDay, st.wHour*60 + st.wMinute, st.wSecond, st.wMilliseconds, k);
				m_text += str2;
				m_text += "SEQUENCE:1\nEND:VEVENT\n";
			}
		}
	}

	m_text += "END:VCALENDAR\n";
	return 1;
}

void FormatTodayInfo(VCTIME vc, CLocation & loc, TString &str)
{
	TString str2, str3, str4;

	int k = 0, nFestClass;

	TResultCalendar db;

	VCTIME vc2 = vc;
	vc2.tzone = loc.m_fTimezone;
	vc2.PreviousDay();
	vc2.PreviousDay();
	vc2.PreviousDay();
	vc2.PreviousDay();
	db.CalculateCalendar(loc, vc2, 9);

	int i = db.FindDate(vc);
	VAISNAVADAY * p = db.GetDay(i);

	if (p == NULL)
		return;

	str2.Format("%s [%s]\r\n\r\n[%d %s %d]\r\n  %s, %s %s\r\n  %s %s, %d Gaurabda\r\n\r\n",
		loc.m_strCity.c_str(), loc.m_strCountry.c_str(), vc.day, AvcGetMonthAbr(vc.month), vc.year,
		GetTithiName(p->astrodata.nTithi), GetPaksaName(p->astrodata.nPaksa), gstr[20].c_str(), 
		GetMasaName(p->astrodata.nMasa), gstr[22].c_str(), p->astrodata.nGaurabdaYear);

	str = str2;

	if (p->ekadasi_parana)
	{
		FormatCalendarDayHeader(p, str2);
		str2.Delete(0, 14);
		str += str2;
		str += "\r\n";
	}

	// adding mahadvadasi
	// adding spec festivals

	if (p->festivals)
	{
		int i = p->GetHeadFestival();
		while(p->GetNextFestival(i, str2))
		{
			if (str2.GetLength() > 1)
			{
				nFestClass = p->GetFestivalClass(str2);
				if (nFestClass < 0 || GetShowSetVal(22 + nFestClass) == 1)
				{
					str += "   ";
					str += str2;
					str += "\r\n";
				}
			}
		}
	}

	str += "\r\n";

	if (GetShowSetVal(16) == 1 && p->sankranti_zodiac >= 0)
	{
		//double h1, m1;
		//m1 = modf(p->sankranti_day.shour*24, &h1);
		str2.Format(" %s %s (%s %s %s %d %s, %02d:%02d %s) "
			, GetSankrantiName(p->sankranti_zodiac)
			, gstr[56].c_str()
			, gstr[111].c_str(), GetSankrantiNameEn(p->sankranti_zodiac)
			, gstr[852].c_str()
			, p->sankranti_day.day, AvcGetMonthAbr(p->sankranti_day.month)
			, p->sankranti_day.GetHour(), p->sankranti_day.GetMinuteRound()
			, AvcGetDSTSignature(p->nDST));
		int length = str2.GetLength();
		int seplen = strlen(gpszSeparator);
		length = (seplen - length) / 2;
		str += (gpszSeparator + (seplen - length));
		str += str2;
		str += (gpszSeparator + (seplen - length));
		str += "\r\n";
	}

	if (GetShowSetVal(7) == 1 && p->was_ksaya)//(m_dshow.m_info_ksaya) && (pvd->was_ksaya))
	{
		double h, m;
		VCTIME dd;
		m = modf(p->ksaya_time1*24, &h);
		dd = p->date;
		if (p->ksaya_day1 < 0.0)
			dd.PreviousDay();
		str2.Format("%d %s, %02d:%02d", dd.day, AvcGetMonthAbr(dd.month), int(h), int(m*60));

//				if (pvd->ksaya_time2 < 0.0)
//					m = modf(fabs(1.0 + pvd->ksaya_time2)*24, &h);
//				else
		m = modf(p->ksaya_time2*24, &h);
		dd = p->date;
		if (p->ksaya_day2 < 0.0)
			dd.PreviousDay();
		str3.Format("%d %s, %02d:%02d", dd.day, AvcGetMonthAbr(dd.month), int(h), int(m*60));

		str4.Format("%s %s %s %s %s (%s)\r\n", gstr[89].c_str(), gstr[850].c_str(), str2.c_str(), gstr[851].c_str(), str3.c_str(), AvcGetDSTSignature(p->nDST));
		str += str4;
	}
	// adding fasting
			if (GetShowSetVal(8) == 1)//(m_dshow.m_info_vriddhi) && (pvd->is_vriddhi))
			{
				if (p->is_vriddhi)
				{
					str += gstr[90];
					str += "\r\n";
				}
			}


	if (p->nCaturmasya & CMASYA_PURN_MASK)
	{
		str2.Format("%s [PURNIMA SYSTEM]\r\n"
			, gstr[107 + (p->nCaturmasya & CMASYA_PURN_MASK_DAY)
					   + ((p->nCaturmasya & CMASYA_PURN_MASK_MASA) >> 2)].c_str()
			);
		str += str2;
		if ((p->nCaturmasya & CMASYA_PURN_MASK_DAY) == 0x1)
		{
			str += gstr[110 + ((p->nCaturmasya & CMASYA_PURN_MASK_MASA) >> 2)].c_str();
		}
	}

	if (p->nCaturmasya & CMASYA_PRAT_MASK)
	{
		str2.Format("%s [PRATIPAT SYSTEM]\r\n"
			, gstr[107 + ((p->nCaturmasya & CMASYA_PRAT_MASK_DAY) >> 8)
					   + ((p->nCaturmasya & CMASYA_PRAT_MASK_MASA) >> 10)].c_str()
			);
		str += str2;
		if ((p->nCaturmasya & CMASYA_PRAT_MASK_DAY) == 0x100)
		{
			str += gstr[110 + ((p->nCaturmasya & CMASYA_PRAT_MASK_MASA) >> 10)].c_str();
			str += "\r\n";
		}
	}

	if (p->nCaturmasya & CMASYA_EKAD_MASK)
	{
		str2.Format("%s [EKADASI SYSTEM]\r\n"
			, gstr[107 + ((p->nCaturmasya & CMASYA_EKAD_MASK_DAY) >> 16)
					   + ((p->nCaturmasya & CMASYA_EKAD_MASK_MASA) >> 18)].c_str()
			);
		str += str2;
		if ((p->nCaturmasya & CMASYA_EKAD_MASK_DAY) == 0x10000)
		{
			str += gstr[110 + ((p->nCaturmasya & CMASYA_EKAD_MASK_MASA) >> 18)].c_str();
			str += "\r\n";
		}
	}
			// tithi at arunodaya
			if (GetShowSetVal(0) == 1)//m_dshow.m_tithi_arun)
			{
				str2.Format("%s: %s\r\n", gstr[98].c_str(), GetTithiName(p->astrodata.nTithiArunodaya));
				str += str2;
			}

			//"Arunodaya Time",//1
			if (GetShowSetVal(1) == 1)//m_dshow.m_arunodaya)
			{
				str2.Format("%s %d:%02d (%s)\r\n", gstr[99].c_str(), p->astrodata.sun.arunodaya.hour
					, p->astrodata.sun.arunodaya.min, AvcGetDSTSignature(p->nDST));
				str += str2;
			}

			//"Moonrise Time",//4
			if (GetShowSetVal(4) == 1)
			{
				if (p->moonrise.hour < 0)
					str2 = gstr[136];
				else
				{
					str2.Format("%s %d:%02d (%s)", gstr[53].c_str(), p->moonrise.hour
						, p->moonrise.min, AvcGetDSTSignature(p->nDST));
				}
				str += str2;
				str += "\r\n";
			}
			//"Moonset Time",//5
			if (GetShowSetVal(5) == 1)
			{
				if (p->moonrise.hour < 0)
					str2 = gstr[137];
				else
				{
					str2.Format("%s %d:%02d (%s)", gstr[54].c_str(), p->moonset.hour
						, p->moonset.min, AvcGetDSTSignature(p->nDST));
				}
				str += str2;
				str += "\r\n";
			}
			///"Sun Longitude",//9
			if (GetShowSetVal(9) == 1)//m_dshow.m_sun_long)
			{
				str2.Format("%s: %f (*)\r\n", gstr[100].c_str(), p->astrodata.sun.longitude_deg);
				str += str2;
			}
			//"Moon Longitude",//10
			if (GetShowSetVal(10) == 1)//m_dshow.m_sun_long)
			{
				str2.Format("%s: %f (*)\r\n", gstr[101].c_str(), p->astrodata.moon.longitude_deg);
				str += str2;
			}
			//"Ayanamsha value",//11
			if (GetShowSetVal(11) == 1)//m_dshow.m_sun_long)
			{
				str2.Format("%s %f (%s) (*)\r\n", gstr[102].c_str(), p->astrodata.msAyanamsa, GetAyanamsaName(GetAyanamsaType()));
				str += str2;
			}
			//"Julian Day",//12
			if (GetShowSetVal(12) == 1)//m_dshow.m_sun_long)
			{
				str2.Format("%s %f (*)\r\n", gstr[103].c_str(), p->astrodata.jdate);
				str += str2;
			}

	str2.Format("\r\n%s %d:%02d %s, %s: %s, %s: %s, %s: %s.\r\n",
		gstr[51].c_str(), 
		p->astrodata.sun.rise.hour, p->astrodata.sun.rise.min, 
		AvcGetDSTSignature(p->nDST),
		gstr[15].c_str(), GetNaksatraName(p->astrodata.nNaksatra),
		gstr[104].c_str(), GetYogaName(p->astrodata.nYoga), 
		gstr[105].c_str(), GetSankrantiName(p->astrodata.nRasi));
	str += str2;
	str2.Format("%s %d:%02d %s\r\n", gstr[52].c_str(), p->astrodata.sun.set.hour, p->astrodata.sun.set.min, AvcGetDSTSignature(p->nDST));
	str += str2;


}

void CalcAppDay(CLocation &location, VCTIME event, TResultApp & app)
{
	//MOONDATA moon;
	//SUNDATA sun;
	DAYDATA &d = app.details;
	double dd;
	TString str;
	VCTIME vc = event;
	VCTIME vcsun = event;
	EARTHDATA m_earth = (EARTHDATA)location;

	vc.tzone = location.m_fTimezone;
	app.b_adhika = false;
	app.event = event;
	app.location = location;
	
	vcsun.shour -= location.m_fTimezone/24.0;
	vcsun.NormalizeValues();
	vcsun.tzone = 0.0;
	SunPosition(vcsun, m_earth, d.sun, vcsun.shour - 0.5);
	//SunPosition(vc, m_earth, d.sun, vc.shour - 0.5 - vc.tzone/24.0);
	MoonCalc(vc.GetJulianComplete(), d.moon, m_earth);
	d.msDistance = put_in_360( d.moon.longitude_deg - d.sun.longitude_deg - 180.0);
	d.msAyanamsa = GetAyanamsa( vc.GetJulianComplete() );

	// tithi
	dd = d.msDistance / 12.0;
	d.nTithi = int(floor(dd));
	d.nTithiElapse = (dd - floor(dd)) * 100.0;
	d.nPaksa = (d.nTithi >= 15) ? 1 : 0;


	// naksatra
	dd = put_in_360( d.moon.longitude_deg - d.msAyanamsa );
	dd = ( dd * 3.0) / 40.0;
	d.nNaksatra = int(floor(dd));
	d.nNaksatraElapse = (dd - floor(dd)) * 100.0;
	d.nMasa = MasaCalc(vc, d, m_earth, d.nGaurabdaYear);
	d.nRasi = GetRasi(d.moon.longitude_deg, d.msAyanamsa);

	if (d.nMasa == ADHIKA_MASA)
	{
		d.nMasa = GetRasi(d.sun.longitude_deg, d.msAyanamsa);
		app.b_adhika = true;
	}

	vc.Today();
	vc.tzone = m_earth.tzone;
	int m = 0;
	VATIME va;
	VCTIME vctemp;

	va.tithi = d.nTithi;
	va.masa  = d.nMasa;
	va.gyear = GetGaurabdaYear(vc, m_earth);
	if (va.gyear < d.nGaurabdaYear)
		va.gyear = d.nGaurabdaYear;


	for(int i = 0; i < 6; i++)
	{
		VATIMEtoVCTIME(va, vctemp, m_earth);
		if (va.gyear > d.nGaurabdaYear)
		{
			if (m < TRESULT_APP_CELEBS)
			{
				app.celeb_date[m] = vctemp;
				app.celeb_gy[m] = va.gyear;
				m++;
			}
		}
		va.gyear++;
	}
}

void FormatAppDayText(TResultApp & app, TString &strResult)
{
	//MOONDATA moon;
	//SUNDATA sun;
	DAYDATA &d = app.details;
	TString str;
	VCTIME vc = app.event;
	EARTHDATA m_earth = (EARTHDATA)app.location;
	TString & strText = strResult;
//	int nHour, nMin;
	
//	nHour = int(vc.shour * 24);
//	nMin = int((vc.shour*24 - nHour)*60);

	AddTextLine(strText, gstr[25]);
	AddTextLine(strText, "");

	str.Format("%15s : %d %s %d", gstr[7].c_str(), vc.day, AvcGetMonthAbr(vc.month), vc.year);
	AddTextLine(strText, str);

	str.Format("%15s : %d:%02d", gstr[8].c_str(), vc.GetHour(), vc.GetMinuteRound());
	AddTextLine(strText, str);
	AddTextLine(strText, "");

	str.Format("%15s : %s", gstr[9].c_str(), app.location.m_strCity.c_str());
	AddTextLine(strText, str);
	str.Format("%15s : %s", gstr[10].c_str(), AvcGetTextLatitude(app.location.m_fLatitude));
	AddTextLine(strText, str);
	str.Format("%15s : %s", gstr[11].c_str(), AvcGetTextLongitude(app.location.m_fLongitude));
	AddTextLine(strText, str);
	str.Format("%15s : ", gstr[12].c_str());
	str += AvcGetTextTimeZone(app.location.m_fTimezone);
	str += "  ";
	str += TTimeZone::GetTimeZoneName(app.location.m_nDST);
	AddTextLine(strText, str);
	str.Format("%15s : N/A", "DST");
	AddTextLine(strText, str);
	AddTextLine(strText, "");

	str.Format("%15s : %s", gstr[13].c_str(), GetTithiName(d.nTithi));
	AddTextLine(strText, str);
	str.Format("%15s : %.2f %%", gstr[14].c_str(), d.nTithiElapse);
	AddTextLine(strText, str);
	str.Format("%15s : %s", gstr[15].c_str(), GetNaksatraName(d.nNaksatra));
	AddTextLine(strText, str);
	str.Format("%15s : %.2f %%", gstr[16].c_str(), d.nNaksatraElapse);
	AddTextLine(strText, str);

	if (g_BhanuMode == 1)
	{
		AddTextLine(strText, "--------");
		str.Format("%15s : %s... %s", gstr[17].c_str() , GetNaksatraChildSylable(d.nNaksatra, int(d.nNaksatraElapse/25.0)), gstr[18].c_str());
		AddTextLine(strText, str);
		str.Format("%15s : %s... %s", gstr[17].c_str(), GetRasiChildSylable(d.nRasi), gstr[19].c_str());
		AddTextLine(strText, str);
		AddTextLine(strText, "--------");
	}

	str.Format("%15s : %s", gstr[20].c_str(), GetPaksaName(d.nPaksa));
	AddTextLine(strText, str);
	if (app.b_adhika == true)
	{
		str.Format("%15s : %s %s", gstr[22].c_str(), GetMasaName(d.nMasa), gstr[21].c_str());
	}
	else
		str.Format("%15s : %s", gstr[22].c_str(), GetMasaName(d.nMasa));
	AddTextLine(strText, str);
	str.Format("%15s : %d", gstr[23].c_str(), d.nGaurabdaYear);
	AddTextLine(strText, str);

	AddTextLine(strText, "");
	AddTextLine(strText, gstr[24].c_str());
	AddTextLine(strText, "");

	for(int o = 0; o < TRESULT_APP_CELEBS; o++)
	{
		str.Format("   Gaurabda %3d : %d %s %d", app.celeb_gy[o] , app.celeb_date[o].day,
			AvcGetMonthAbr(app.celeb_date[o].month), 
			app.celeb_date[o].year);
		AddTextLine(strText, str);
	}

}

void FormatAppDayXML(TResultApp &app, TString &strResult)
{
	DAYDATA &d = app.details;
	TString str;
	VCTIME vc = app.event;
	EARTHDATA m_earth = (EARTHDATA)app.location;
	CLocation & loc = app.location;
	TString & strText = strResult;
	int npada;
	Boolean bDuringAdhika = app.b_adhika;

	strText.Format(
		"<xml>\n"
		"\t<request name=\"AppDay\" version=\"%s\">\n"
		"\t\t<arg name=\"longitude\" value=\"%+f\" />\n"
		"\t\t<arg name=\"latitude\" value=\"%+f\" />\n"
		"\t\t<arg name=\"timezone\" value=\"%+f\" />\n"
		"\t\t<arg name=\"year\" value=\"%d\" />\n"
		"\t\t<arg name=\"month\" value=\"%d\" />\n"
		"\t\t<arg name=\"day\" value=\"%d\" />\n"
		"\t\t<arg name=\"hour\" value=\"%d\" />\n"
		"\t\t<arg name=\"minute\" value=\"%d\" />\n"
		"\t</request>\n", gstr[130].c_str(),
		loc.m_fLongitude, loc.m_fLatitude, loc.m_fTimezone,
		app.event.year, app.event.month, app.event.day, app.event.GetHour(), app.event.GetMinuteRound()
		);


	npada = int (d.nNaksatraElapse / 25.0) + 1;
	if (npada > 4)
		npada = 4;

	str.Format("\t<result name=\"AppDay\" >\n"
		"\t\t<tithi name=\"%s\" elapse=\"%f\" />\n"
		"\t\t<naksatra name=\"%s\" elapse=\"%f\" pada=\"%d\"/>\n"
		"\t\t<paksa name=\"%s\" />\n"
		"\t\t<masa name=\"%s\" adhikamasa=\"%s\"/>\n"
		"\t\t<gaurabda value=\"%d\" />\n"

		, GetTithiName(d.nTithi), d.nTithiElapse
		, GetNaksatraName(d.nNaksatra), d.nNaksatraElapse, npada
		, GetPaksaName(d.nPaksa)
		, GetMasaName(d.nMasa), (bDuringAdhika ? "yes" : "no")
		, d.nGaurabdaYear
		);

	strText += str;

	strText += "\t\t<celebrations>\n";
	for(int i =0; i < TRESULT_APP_CELEBS; i++)
	{
		str.Format("\t\t\t<celebration gaurabda=\"%d\" day=\"%d\" month=\"%d\" monthabr=\"%s\" year=\"%d\" />\n"
			, app.celeb_gy[i], app.celeb_date[i].day, app.celeb_date[i].month, AvcGetMonthAbr(app.celeb_date[i].month), app.celeb_date[i].year);
		strText += str;
	}

	strText += "\t\t</celebrations>\n\t</result>\n</xml>\n";
}

int CalcMasaList(TResultMasaList &mlist, CLocation &loc, int nYear, int nCount)
{
	DAYDATA day;
	VCTIME d, de, t;
	int lm = -1;

	mlist.n_startYear = nYear;
	mlist.n_countYears = nCount;
	mlist.vc_start = d  = GetFirstDayOfYear((EARTHDATA)loc, nYear);
	mlist.vc_end   = de = GetFirstDayOfYear((EARTHDATA)loc, nYear + nCount);
	mlist.m_location = loc;

	if (mlist.alloc(nCount) == false)
		return 0;

	int i = 0;
	int prev_paksa = -1;
	int prev_gyear = -1;
	int current = 0;


	EARTHDATA earth = (EARTHDATA)loc;
	ProgressWindowCreate();
	while(d.IsBeforeThis(de))
	{
		DayCalc(d, earth, day);
		if (prev_paksa != day.nPaksa)
		{
			day.nMasa = MasaCalc(d, day, earth, day.nGaurabdaYear);
			
			if (lm != day.nMasa)
			{
				if (lm >= 0)
				{
					t = d;
					t.PreviousDay();
					mlist.arr[current].vc_end = t;
					current++;
				}
				lm = day.nMasa;
				mlist.arr[current].masa = day.nMasa;
				mlist.arr[current].year = day.nGaurabdaYear;
				mlist.arr[current].vc_start = d;
			}
		}
		prev_paksa = day.nPaksa;
		d.NextDay();
		i ++;
		ProgressWindowSetPos(i / (3 * nCount));
	}

	t = d;
	mlist.arr[current].vc_end = t;
	current++;
	mlist.n_countMasa = current;

	ProgressWindowClose();
	return 1;
}

int FormatMasaListText(TResultMasaList &mlist, TString &str)
{
	TString stt;
	TString stt2;

	str.Format(" %s\r\n\r\n%s: %s\r\n", gstr[39].c_str(), gstr[40].c_str(), mlist.m_location.getFullName());
	stt.Format("%s %d %s %d %s %d %s %d\r\n", gstr[41].c_str(), mlist.vc_start.day, AvcGetMonthAbr(mlist.vc_start.month), mlist.vc_start.year
		, gstr[42].c_str(), mlist.vc_end.day, AvcGetMonthAbr(mlist.vc_end.month), mlist.vc_end.year);
	str += stt;
	str += "==================================================================\r\n\r\n";

	int i;

	for(i = 0; i < mlist.n_countMasa; i++)
	{
		stt.Format("%s %d                               ", GetMasaName(mlist.arr[i].masa), mlist.arr[i].year);
		stt.Left(30, stt2);
		str += stt2;
		stt.Format("   %d %s %d - ", mlist.arr[i].vc_start.day, AvcGetMonthAbr(mlist.arr[i].vc_start.month), mlist.arr[i].vc_start.year);
		stt.Right(16, stt2);
		str += stt2;
		stt.Format("   %d %s %d\r\n", mlist.arr[i].vc_end.day, AvcGetMonthAbr(mlist.arr[i].vc_end.month), mlist.arr[i].vc_end.year);
		stt.Right(13, stt2);
		str += stt2;
	}

	return 1;
}
