//////////////////////////////////////////////////////////////////////
// PORTABLE 

#include "astro.h"
#include "avc.h"

#pragma GCC diagnostic ignored "-Wconversion-null"

/*

   Complete calculation of Vaisnava Calendar

Main func is TResultCalendar::CalculateCalendar

*/



typedef char * pchar;
int GetShowSetVal(int i);

int is_daylight_time(VCTIME vc, int nIndex);
void CalcMoonTimes(EARTHDATA e, VCTIME vc, double nDaylightSavingShift, DAYTIME &rise, DAYTIME &set);
int GetSankrantiType();

#define FOREACH_INDEX(i) for(i=0;i<nTotalCount;i++)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TResultCalendar::TResultCalendar()
{
	nTop = 0;
	nBeg = 0;
	m_pData = NULL;
	m_PureCount = 0;
	m_nCount = 0;
}

TResultCalendar::~TResultCalendar()
{
	// dealokacia pola
	if (m_pData != NULL)
	{
		delete [] m_pData;
	}
}

int TResultCalendar::Push(VAISNAVADAY &day)
{
	int nWrite = 0;
	if (nTop < CDB_MAXDAYS - 1)
	{
		nWrite = nTop;
		days[nTop] = day;
		nTop++;
	}
	else
	{
		nWrite = nBeg;
		days[nBeg] = day;
		nBeg = (nBeg + 1) % CDB_MAXDAYS;
	}

	return nWrite;
}


int AddFestString(pchar &pFest, char *newString)
{
	if (newString == NULL)
		return 0;

	if (pFest == NULL)
	{
		pFest = new char[strlen(newString)];
		strcpy(pFest, newString);
		return 1;
	}

	pchar temp = new char[strlen(pFest) + 3 + strlen(newString)];
	strcpy(temp, pFest);
	strcat(temp, "#");
	strcat(temp, newString);

	delete [] pFest;

	pFest = temp;

	return 1;
}


Boolean TResultCalendar::NextNewFullIsVriddhi(int nIndex, EARTHDATA earth)
{
	int i = 0;
	int nTithi;
	int nPrevTithi = 100;

	for(i = 0; i < BEFORE_DAYS; i++)
	{
		nTithi = m_pData[nIndex].astrodata.nTithi;
		if ((nTithi == nPrevTithi) && TITHI_FULLNEW_MOON(nTithi))
		{
			return true;
		}
		nPrevTithi = nTithi;
		nIndex++;
	}

	return false;
}

// test for MAHADVADASI 5 TO 8

Boolean TResultCalendar::IsMhd58(int nIndex, int &nMahaType)
{
	VAISNAVADAY & t = m_pData[nIndex];
	VAISNAVADAY & u = m_pData[nIndex + 1];

	nMahaType = NULL;

	if (t.astrodata.nNaksatra != u.astrodata.nNaksatra)
		return false;

	if (t.astrodata.nPaksa != 1)
		return false;

	if (t.astrodata.nTithi == t.astrodata.nTithiSunset)
	{
		if (t.astrodata.nNaksatra == 6) // punarvasu
		{
			nMahaType = EV_JAYA;
			return true;
		}
		else if (t.astrodata.nNaksatra == 3) // rohini
		{
			nMahaType = EV_JAYANTI;
			return true;
		}
		else if (t.astrodata.nNaksatra == 7) // pusyami
		{
			nMahaType = EV_PAPA_NASINI;
			return true;
		}
		else if (t.astrodata.nNaksatra == 21) // sravana
		{
			nMahaType = EV_VIJAYA;
			return true;
		}
		else
			return false;
	}
	else
	{
		if (t.astrodata.nNaksatra == 21) // sravana
		{
			nMahaType = EV_VIJAYA;
			return true;
		}
	}

	return false;
}

void SET_PROG(double p)
{
	ProgressWindowSetPos((int)p);
}

/******************************************************************************************/
/* Main fucntion for VCAL calculations                                                    */
/*                                                                                        */
/*                                                                                        */
/******************************************************************************************/


int TResultCalendar::CalculateCalendar(CLocation & loc, VCTIME begDate, int iCount)
{
	int i, m, weekday;
	int nTotalCount = BEFORE_DAYS + iCount + BEFORE_DAYS;
	VCTIME date;
	int nYear;
	EARTHDATA earth;
	int prev_paksa = 0;
	bool bCalcMoon = true; //(GetShowSetVal(4) > 0 || GetShowSetVal(5) > 0);
	//Boolean bCalcMasa[] =
		//{ true, true, false, false, false, false, false, false, false, false, false, false, false, false, true,
		//true, true, false, false, false, false, false, false, false, false, false, false, false, false, true };

	//GCAL_DLOG("BEGIN CalculateCalendar %d.", __LINE__);

	m_nCount = 0;
	m_Location = loc;
	m_vcStart = begDate;
	m_vcCount = iCount;
	earth = (EARTHDATA)loc;

	// dealokacia pola
	if (m_pData != NULL)
	{
		delete [] m_pData;
	}

	// alokacia pola
	m_pData = new VAISNAVADAY[ nTotalCount ];
	if (m_pData == NULL)
		return 0;

	// inicializacia poctovych premennych
	m_nCount = nTotalCount;
	m_PureCount = iCount;

	date = begDate;
	date.shour = 0.0;
	date.tzone = loc.m_fTimezone;
//	date -= BEFORE_DAYS;

	//ProgressWindowCreate();
	//ProgressWindowSetRange(0, 100);

	for(i = BEFORE_DAYS; i > 0; i--)
	{
		date.PreviousDay();
	}

	weekday = (int(date.GetJulian()) + 1) % 7;

	// 1
	//GCAL_DLOG("Initialization of days %d.", __LINE__);

	FOREACH_INDEX(i)
	{
		m_pData[i].date = date;
		m_pData[i].date.dayOfWeek = weekday;
		m_pData[i].fDateValid = true;
		m_pData[i].fVaisValid = false;
		date.NextDay();
		weekday = (weekday + 1) % 7;
		m_pData[i].moonrise.SetValue(-1);
		m_pData[i].moonset.SetValue(-1);
	}

	// 2
	//GCAL_DLOG("Is daylight time %d.", __LINE__);

	//
	FOREACH_INDEX(i)
	{
		m_pData[i].nDST = is_daylight_time(m_pData[i].date, loc.m_nDST);
	}

	// 3
	//GCAL_DLOG("Calculating moon times %d.", __LINE__);

	if (bCalcMoon)
	{
		FOREACH_INDEX(i)
		{
			//SET_PROG( (0 + 85 * i / (iCount + 1)) * 0.908 );
			CalcMoonTimes(earth, m_pData[i].date, double(m_pData[i].nDST), m_pData[i].moonrise, m_pData[i].moonset);
		}
	}

	// 4
	//GCAL_DLOG("Init of astro data %d.", __LINE__);

	//
	FOREACH_INDEX(i)
	{
		if (bCalcMoon)
		{
			//SET_PROG( (85 + 2 * i / nTotalCount) * 0.908);
		}
		else
		{
			//SET_PROG( 0.588 * 14.8 * i / nTotalCount );
		}
		DayCalc(m_pData[i].date, earth, m_pData[i].astrodata);

	}

	Boolean calc_masa;

	// 5
	//GCAL_DLOG("Init of masa %d.", __LINE__);

	//
	prev_paksa = -1;
	FOREACH_INDEX(i)
	{
		calc_masa = (m_pData[i].astrodata.nPaksa != prev_paksa);
		prev_paksa = m_pData[i].astrodata.nPaksa;
		
		if (bCalcMoon)
		{
			//SET_PROG( (87 + 2 * i / nTotalCount) * 0.908 );
		}
		else
		{
			//SET_PROG( 0.588 * (14.8 + 32.2 * i / nTotalCount));
		}

		if (i == 0)
			calc_masa = true;

		/*if (i > 0)
		{
			if ((m_pData[i-1].astrodata.nTithi <=14) && (m_pData[i].astrodata.nTithi >= 15))
			{
				calc_masa = true;
			}
			else if (m_pData[i-1].astrodata.nTithi > m_pData[i].astrodata.nTithi)
			{
				calc_masa = true;
			}
		}*/

		if (calc_masa)
		{
			m = MasaCalc(m_pData[i].date, m_pData[i].astrodata, earth, nYear);
		}
		m_pData[i].astrodata.nMasa = m;
		m_pData[i].astrodata.nGaurabdaYear = nYear;
		m_pData[i].fAstroValid = true;
	}

	// 6
	//GCAL_DLOG("Init of mahadvadasis %d.", __LINE__);

	//
	for(i = 2; i < m_PureCount + BEFORE_DAYS + 3; i++)
	{
		m_pData[i].Clear();
		MahadvadasiCalc(i, earth);
	}

	// 6,5
	//GCAL_DLOG("Init for Ekadasis %d.", __LINE__);

	//
	for(i = 3; i < m_PureCount + BEFORE_DAYS + 3; i++)
	{
		if (bCalcMoon)
		{
			//SET_PROG( (89 + 5 * i / nTotalCount) * 0.908 );
		}
		else
		{
			//SET_PROG( 0.588 * (47.0 + 39.5 * i / nTotalCount));
		}
		EkadasiCalc(i, earth);
	}

	// 7
	//GCAL_DLOG("Init of festivals %d.", __LINE__);

	//
	for(i = BEFORE_DAYS; i < m_PureCount + BEFORE_DAYS + 3; i++)
	{
		CompleteCalc(i, earth);
	}

	// 8
	//GCAL_DLOG("Init of festivals %d.", __LINE__);

	//
	for(i = BEFORE_DAYS; i < m_PureCount + BEFORE_DAYS; i++)
	{
		ExtendedCalc(i, earth);
	}

	// resolve festivals fasting
	for(i = BEFORE_DAYS; i < m_PureCount + BEFORE_DAYS; i++)
	{
		if (m_pData[i].eparana_time1 > 0.0)
		{
			m_pData[i].eparana_time1 += m_pData[i].nDST;
		}

		if (m_pData[i].eparana_time2 > 0.0)
		{
			m_pData[i].eparana_time2 += m_pData[i].nDST;
		}

		if (m_pData[i].astrodata.sun.longitude_deg > 0.0)
		{
			m_pData[i].astrodata.sun.rise.hour += m_pData[i].nDST;
			m_pData[i].astrodata.sun.set.hour += m_pData[i].nDST;
			m_pData[i].astrodata.sun.noon.hour += m_pData[i].nDST;
			m_pData[i].astrodata.sun.arunodaya.hour += m_pData[i].nDST;
		}

		ResolveFestivalsFasting(i);
	}

	//
	//GCAL_DLOG("Init for sankranti %d.", __LINE__);

	date = m_pData[0].date;
	i = 0;
	bool bFoundSan;
	int zodiac;
	int i_target;
	do
	{
		date = GetNextSankranti(date, zodiac);
		date.shour += is_daylight_time(date, loc.m_nDST)/24.0;
		date.NormalizeValues();

		bFoundSan = false;
		for(i=0;i < m_nCount-1;i++)
		{
			i_target = -1;

			switch(GetSankrantiType())
			{
			case 0:
				if (date.CompareYMD(m_pData[i].date) == 0)
				{
					i_target = i;
				}
				break;
			case 1:
				if (date.CompareYMD(m_pData[i].date) == 0)
				{
					if (date.shour < m_pData[i].astrodata.sun.rise.GetDayTime())
					{
						i_target = i - 1;
					}
					else
					{
						i_target = i;
					}
				}
				break;
			case 2:
				if (date.CompareYMD(m_pData[i].date) == 0)
				{
					if (date.shour > m_pData[i].astrodata.sun.noon.GetDayTime())
					{
						i_target = i+1;
					}
					else
					{
						i_target = i;
					}
				}
				break;
			case 3:
				if (date.CompareYMD(m_pData[i].date) == 0)
				{
					if (date.shour > m_pData[i].astrodata.sun.set.GetDayTime())
					{
						i_target = i+1;
					}
					else
					{
						i_target = i;
					}
				}
				break;
			}

			if (i_target >= 0)
			{
				m_pData[i_target].sankranti_zodiac = zodiac;
				m_pData[i_target].sankranti_day = date;
				bFoundSan = true;
				break;
			}
		}
		date.NextDay();
		date.NextDay();
	}
	while(bFoundSan == true);

	// 9
	//GCAL_DLOG("Init for festivals dependent on sankranti %d.", __LINE__);

	//
	for(i = BEFORE_DAYS; i < m_PureCount + BEFORE_DAYS; i++)
	{
		if (m_pData[i].sankranti_zodiac == MAKARA_SANKRANTI)
		{
			if (m_pData[i].festivals.IsEmpty() == false)
				m_pData[i].festivals += "#";
			m_pData[i].festivals += "[c5]";
			m_pData[i].festivals += gstr[78];
		}
		else if (m_pData[i].sankranti_zodiac == MESHA_SANKRANTI)
		{
			if (m_pData[i].festivals.IsEmpty() == false)
				m_pData[i].festivals += "#";
			m_pData[i].festivals += "[c5]";
			m_pData[i].festivals += gstr[79];
		}
		else if (m_pData[i+1].sankranti_zodiac == VRSABHA_SANKRANTI)
		{
			if (m_pData[i].festivals.IsEmpty() == false)
				m_pData[i].festivals += "#";
			m_pData[i].festivals += "[c5]";
			m_pData[i].festivals += gstr[80];
		}
	}

	// 10
	//GCAL_DLOG("init ksaya and second day of vriddhi  %d.", __LINE__);

	//
	for(i = BEFORE_DAYS; i < m_PureCount + BEFORE_DAYS; i++)
	{
		if (bCalcMoon)
		{
			//SET_PROG( (94 + 6 * i / nTotalCount) * 0.908 );
		}
		else
		{
			//SET_PROG( 0.588 * (86.5 + 13.5 * i / nTotalCount));
		}
		if (m_pData[i].astrodata.nTithi == m_pData[i-1].astrodata.nTithi)
			m_pData[i].is_vriddhi = true;
		else if (m_pData[i].astrodata.nTithi != ((m_pData[i-1].astrodata.nTithi + 1)%30))
		{
			m_pData[i].was_ksaya = true;

			VCTIME day1, d1, d2;
			day1 = m_pData[i].date;
			day1.shour = m_pData[i].astrodata.sun.sunrise_deg/360.0 + earth.tzone/24.0;

			GetPrevTithiStart(earth, day1, d2);
			day1 = d2;
			day1.shour -= 0.1;
			day1.NormalizeValues();
			GetPrevTithiStart(earth, day1, d1);

			d1.shour += (m_pData[i].nDST/24.0);
			d2.shour += (m_pData[i].nDST/24.0);

			d1.NormalizeValues();
			d2.NormalizeValues();

			m_pData[i].ksaya_day1 = (d1.day == m_pData[i].date.day) ? 0 : -1;
			m_pData[i].ksaya_time1 = d1.shour;
			m_pData[i].ksaya_day2 = (d2.day == m_pData[i].date.day) ? 0 : -1;
			m_pData[i].ksaya_time2 = d2.shour;

		}
	}

	//GCAL_DLOG("END CalculateCalendar %d.", __LINE__);
	return 1;
}

#define MAX_VAL(a,b) ((a) > (b) ? (a) : (b))
#define MIN_VAL(a,b) ((a) < (b) ? (a) : (b))


int TResultCalendar::EkadasiCalc(int nIndex, EARTHDATA earth)
{
	VAISNAVADAY & s = m_pData[nIndex-1];
	VAISNAVADAY & t = m_pData[nIndex];
	VAISNAVADAY & u = m_pData[nIndex+1];

	if (TITHI_EKADASI(t.astrodata.nTithi))
	{
		// if TAT < 11 then NOT_EKADASI
		if (TITHI_LESS_EKADASI(t.astrodata.nTithiArunodaya))
		{
			t.nMhdType = EV_NULL;
			t.ekadasi_vrata_name.Empty();
			t.nFastType = FAST_NULL;
		}
		else 
		{
			// else ak MD13 then MHD1 and/or 3
			if (TITHI_EKADASI(s.astrodata.nTithi) && TITHI_EKADASI(s.astrodata.nTithiArunodaya))
			{
				if (TITHI_TRAYODASI(u.astrodata.nTithi))
				{
					t.nMhdType = EV_UNMILANI_TRISPRSA;
					t.ekadasi_vrata_name = GetEkadasiName(t.astrodata.nMasa, t.astrodata.nPaksa);
					t.nFastType = FAST_EKADASI;
				}
				else
				{
					t.nMhdType = EV_UNMILANI;
					t.ekadasi_vrata_name = GetEkadasiName(t.astrodata.nMasa, t.astrodata.nPaksa);
					t.nFastType = FAST_EKADASI;
				}
			}
			else
			{
				if (TITHI_TRAYODASI(u.astrodata.nTithi))
				{
					t.nMhdType = EV_TRISPRSA;
					t.ekadasi_vrata_name = GetEkadasiName(t.astrodata.nMasa, t.astrodata.nPaksa);
					t.nFastType = FAST_EKADASI;
				}
				else
				{
					// else ak U je MAHADVADASI then NOT_EKADASI
					if (TITHI_EKADASI(u.astrodata.nTithi) || (u.nMhdType >= EV_SUDDHA))
					{
						t.nMhdType = EV_NULL;
						t.ekadasi_vrata_name.Empty();
						t.nFastType = FAST_NULL;
					}
					else if (u.nMhdType == EV_NULL)
					{
						// else suddha ekadasi
						t.nMhdType = EV_SUDDHA;
						t.ekadasi_vrata_name = GetEkadasiName(t.astrodata.nMasa, t.astrodata.nPaksa);
						t.nFastType = FAST_EKADASI;
					}
				}
			}
		}
	}
	// test for break fast

	if (s.nFastType == FAST_EKADASI)
	{
		double parBeg, parEnd;

		CalculateEParana(s, t, parBeg, parEnd, earth);

	}

	return 1;
}


int ctoi(char c)
{
	if ((c >= '0') && (c <= '9'))
		return c - '0';
	else
		return 0;
}

/******************************************************************************************/
/*                                                                                        */
/*                                                                                        */
/*                                                                                        */
/******************************************************************************************/

int TResultCalendar::CompleteCalc(int nIndex, EARTHDATA earth)
{
	VAISNAVADAY & s = m_pData[nIndex-1];
	VAISNAVADAY & t = m_pData[nIndex];
	VAISNAVADAY & u = m_pData[nIndex+1];
	VAISNAVADAY & v = m_pData[nIndex+2];

	/*if (TITHI_EKADASI(t.astrodata.nTithi))
	{
		// if TAT < 11 then NOT_EKADASI
		if (TITHI_LESS_EKADASI(t.astrodata.nTithiArunodaya))
		{
			t.nMhdType = EV_NULL;
			t.ekadasi_vrata_name.Empty();
			t.nFastType = FAST_NULL;
		}
		else 
		{
			// else ak MD13 then MHD1 and/or 3
			if (TITHI_EKADASI(s.astrodata.nTithi) && TITHI_EKADASI(s.astrodata.nTithiArunodaya))
			{
				if (TITHI_TRAYODASI(u.astrodata.nTithi))
				{
					t.nMhdType = EV_UNMILANI_TRISPRSA;
					t.ekadasi_vrata_name = GetEkadasiName(t.astrodata.nMasa, t.astrodata.nPaksa);
					t.nFastType = FAST_EKADASI;
				}
				else
				{
					t.nMhdType = EV_UNMILANI;
					t.ekadasi_vrata_name = GetEkadasiName(t.astrodata.nMasa, t.astrodata.nPaksa);
					t.nFastType = FAST_EKADASI;
				}
			}
			else
			{
				if (TITHI_TRAYODASI(u.astrodata.nTithi))
				{
					t.nMhdType = EV_TRISPRSA;
					t.ekadasi_vrata_name = GetEkadasiName(t.astrodata.nMasa, t.astrodata.nPaksa);
					t.nFastType = FAST_EKADASI;
				}
				else
				{
					// else ak U je MAHADVADASI then NOT_EKADASI
					if (TITHI_EKADASI(u.astrodata.nTithi) || (u.nMhdType >= EV_SUDDHA))
					{
						t.nMhdType = EV_NULL;
						t.ekadasi_vrata_name.Empty();
						t.nFastType = FAST_NULL;
					}
					else if (u.nMhdType == EV_NULL)
					{
						// else suddha ekadasi
						t.nMhdType = EV_SUDDHA;
						t.ekadasi_vrata_name = GetEkadasiName(t.astrodata.nMasa, t.astrodata.nPaksa);
						t.nFastType = FAST_EKADASI;
					}
				}
			}
		}
	}
	// test for break fast

	if (s.nFastType == FAST_EKADASI)
	{
		double parBeg, parEnd;

		CalculateEParana(s, t, parBeg, parEnd, earth);

	}*/

	// test for Govardhan-puja
	if (t.astrodata.nMasa == DAMODARA_MASA)
	{
		if (t.astrodata.nTithi == TITHI_GAURA_PRATIPAT)
		{
			CalcMoonTimes(earth, u.date, s.nDST, s.moonrise, s.moonset);
			CalcMoonTimes(earth, t.date, t.nDST, t.moonrise, t.moonset);
			if (s.astrodata.nTithi == TITHI_GAURA_PRATIPAT)
			{
			}
			else if (u.astrodata.nTithi == TITHI_GAURA_PRATIPAT)
			{
				if (t.moonrise.hour >= 0)
				{
					if (t.moonrise > t.astrodata.sun.rise)
					// today is GOVARDHANA PUJA
						AddSpecFestival(t, SPEC_GOVARDHANPUJA, "[c1]");
					else
						AddSpecFestival(u, SPEC_GOVARDHANPUJA, "[c1]");
				}
				else if (u.moonrise.hour >= 0)
				{
					if (u.moonrise < u.astrodata.sun.rise)
					// today is GOVARDHANA PUJA
						AddSpecFestival(t, SPEC_GOVARDHANPUJA, "[c1]");
					else
						AddSpecFestival(u, SPEC_GOVARDHANPUJA, "[c1]");
				}
				else
				{
					AddSpecFestival(t, SPEC_GOVARDHANPUJA, "[c1]");
				}
			}
			else
			{
				// today is GOVARDHANA PUJA
				AddSpecFestival(t, SPEC_GOVARDHANPUJA, "[c1]");
			}

		}
		else if ((t.astrodata.nTithi == TITHI_GAURA_DVITIYA) && (s.astrodata.nTithi == TITHI_AMAVASYA))
		{
			// today is GOVARDHANA PUJA
			AddSpecFestival(t, SPEC_GOVARDHANPUJA, "[c1]");
		}
	}

	int mid_nak_t, mid_nak_u;

	if (t.astrodata.nMasa == HRSIKESA_MASA)
	{
		// test for Janmasthami
		if (IsFestivalDay(s, t, TITHI_KRSNA_ASTAMI))
		{
			// if next day is not astami, so that means that astami is not vriddhi
			// then today is SKJ
			if (u.astrodata.nTithi != TITHI_KRSNA_ASTAMI)
			{
				// today is Sri Krsna Janmasthami
				AddSpecFestival(t, SPEC_JANMASTAMI, "[c0]");
				AddSpecFestival(u, SPEC_NANDAUTSAVA, "[c1]");
				AddSpecFestival(u, SPEC_PRABHAPP, "[c2]");
//				t.nFastType = FAST_MIDNIGHT;
			}
			else // tithi is vriddhi and we have to test both days
			{
				// test when both days have ROHINI
				if ((t.astrodata.nNaksatra == ROHINI_NAKSATRA) && (u.astrodata.nNaksatra == ROHINI_NAKSATRA))
				{
					mid_nak_t = (int)DayCalcEx(t.date, earth, DCEX_NAKSATRA_MIDNIGHT);
					mid_nak_u = (int)DayCalcEx(u.date, earth, DCEX_NAKSATRA_MIDNIGHT);

					// test when both days have modnight naksatra ROHINI
					if ((ROHINI_NAKSATRA == mid_nak_u) && (mid_nak_t == ROHINI_NAKSATRA))
					{
						// choice day which is monday or wednesday
						if ((u.date.dayOfWeek == DW_MONDAY) || (u.date.dayOfWeek == DW_WEDNESDAY))
						{
							AddSpecFestival(u, SPEC_JANMASTAMI, "[c0]");
							AddSpecFestival(v, SPEC_NANDAUTSAVA, "[c1]");
							AddSpecFestival(v, SPEC_PRABHAPP, "[c2]");
//							u.nFastType = FAST_MIDNIGHT;
						}
						else
						{
							// today is Sri Krsna Janmasthami
							AddSpecFestival(t, SPEC_JANMASTAMI, "[c0]");
							AddSpecFestival(u, SPEC_NANDAUTSAVA, "[c1]");
							AddSpecFestival(u, SPEC_PRABHAPP, "[c2]");
//							t.nFastType = FAST_MIDNIGHT;
						}
					}
					else if (mid_nak_t == ROHINI_NAKSATRA)
					{
						// today is Sri Krsna Janmasthami
						AddSpecFestival(t, SPEC_JANMASTAMI, "[c0]");
						AddSpecFestival(u, SPEC_NANDAUTSAVA, "[c1]");
//						t.nFastType = FAST_MIDNIGHT;
						AddSpecFestival(u, SPEC_PRABHAPP, "[c2]");
					}
					else if (mid_nak_u == ROHINI_NAKSATRA)
					{
						AddSpecFestival(u, SPEC_JANMASTAMI, "[c0]");
						AddSpecFestival(v, SPEC_NANDAUTSAVA, "[c1]");
						AddSpecFestival(v, SPEC_PRABHAPP, "[c2]");
//						u.nFastType = FAST_MIDNIGHT;
					}
					else
					{
						if ((u.date.dayOfWeek == DW_MONDAY) || (u.date.dayOfWeek == DW_WEDNESDAY))
						{
							AddSpecFestival(u, SPEC_JANMASTAMI, "[c0]");
							AddSpecFestival(v, SPEC_NANDAUTSAVA, "[c1]");
							AddSpecFestival(v, SPEC_PRABHAPP, "[c2]");
//							u.nFastType = FAST_MIDNIGHT;
						}
						else
						{
							// today is Sri Krsna Janmasthami
							AddSpecFestival(t, SPEC_JANMASTAMI, "[c0]");
							AddSpecFestival(u, SPEC_NANDAUTSAVA, "[c1]");
							AddSpecFestival(u, SPEC_PRABHAPP, "[c2]");
//							t.nFastType = FAST_MIDNIGHT;
						}
					}
				}
				else if (t.astrodata.nNaksatra == ROHINI_NAKSATRA)
				{
					// today is Sri Krsna Janmasthami
					AddSpecFestival(t, SPEC_JANMASTAMI, "[c0]");
					AddSpecFestival(u, SPEC_NANDAUTSAVA, "[c1]");
					AddSpecFestival(u, SPEC_PRABHAPP, "[c2]");
//					t.nFastType = FAST_MIDNIGHT;
				}
				else if (u.astrodata.nNaksatra == ROHINI_NAKSATRA)
				{
					AddSpecFestival(u, SPEC_JANMASTAMI, "[c0]");
					AddSpecFestival(v, SPEC_NANDAUTSAVA, "[c1]");
					AddSpecFestival(v, SPEC_PRABHAPP, "[c2]");
//					u.nFastType = FAST_MIDNIGHT;
				}
				else
				{
					if ((u.date.dayOfWeek == DW_MONDAY) || (u.date.dayOfWeek == DW_WEDNESDAY))
					{
						AddSpecFestival(u, SPEC_JANMASTAMI, "[c0]");
						AddSpecFestival(v, SPEC_NANDAUTSAVA, "[c1]");
						AddSpecFestival(v, SPEC_PRABHAPP, "[c2]");
//						u.nFastType = FAST_MIDNIGHT;
					}
					else
					{
						// today is Sri Krsna Janmasthami
						AddSpecFestival(t, SPEC_JANMASTAMI, "[c0]");
						AddSpecFestival(u, SPEC_NANDAUTSAVA, "[c1]");
						AddSpecFestival(u, SPEC_PRABHAPP, "[c2]");
//						t.nFastType = FAST_MIDNIGHT;
					}
				}
			}
		}

/*		if (s.nSpecFestival == SPEC_JANMASTAMI)
		{
			// today is SP VyasaPuja
			// today is Nandottsava
			AddSpecFestival(t, SPEC_NANDOTSAVA);
//			t.nFastType = FAST_NOON;
		}
*/
		// test for Vamana Dvadasi
/*		if (IsFestivalDay(s, t, TITHI_GAURA_DVADASI))
		{
			// today is Vamana Dvadasi
			t.nSpecFestival = SPEC_VAMANADVADASI;

			if (t.nFastType == FAST_EKADASI)
			{
				t.nFeasting = FEAST_TOMMOROW_FAST_TODAY;
				u.nFeasting = FEAST_TODAY_FAST_YESTERDAY;
			}
			else
			{
				t.nFeasting = FEAST_TODAY_FAST_YESTERDAY;
			}
		}*/
	}

	// test for Varaha Dvadasi
	/*if (t.astrodata.nMasa == MADHAVA_MASA)
	{
		if (((t.astrodata.nTithi == TITHI_GAURA_DVADASI) && (s.astrodata.nTithi < TITHI_GAURA_DVADASI))
			|| ((t.astrodata.nTithi == TITHI_GAURA_EKADASI) && (u.astrodata.nTithi == TITHI_GAURA_TRAYODASI)))
		{
			// today is Varaha Dvadasi
			t.nSpecFestival = SPEC_VARAHADVADASI;

			if (t.nFastType == FAST_EKADASI)
			{
				t.nFeasting = FEAST_TOMMOROW_FAST_TODAY;
				u.nFeasting = FEAST_TODAY_FAST_YESTERDAY;
			}
			else
			{
				t.nFeasting = FEAST_TODAY_FAST_YESTERDAY;
			}
		}
	}*/

	// test for RathaYatra
	if (t.astrodata.nMasa == VAMANA_MASA)
	{
		if (IsFestivalDay(s, t, TITHI_GAURA_DVITIYA))
		{
			AddSpecFestival(t, SPEC_RATHAYATRA, "[c1]");
		}

		if (nIndex > 4)
		{
			if (IsFestivalDay(m_pData[nIndex - 5], m_pData[nIndex - 4], TITHI_GAURA_DVITIYA))
			{
				AddSpecFestival(t, SPEC_HERAPANCAMI, "[c1]");
			}
		}

		if (nIndex > 8)
		{
			if (IsFestivalDay(m_pData[nIndex - 9], m_pData[nIndex - 8], TITHI_GAURA_DVITIYA))
			{
				AddSpecFestival(t, SPEC_RETURNRATHA, "[c1]");
			}
		}

		if (IsFestivalDay(m_pData[nIndex], m_pData[nIndex + 1], TITHI_GAURA_DVITIYA))
		{
			AddSpecFestival(t, SPEC_GUNDICAMARJANA, "[c1]");
		}

	}

	// test for Gaura Purnima
	if (s.astrodata.nMasa == GOVINDA_MASA)
	{
		if (IsFestivalDay(s, t, TITHI_PURNIMA))
		{
			AddSpecFestival(t, SPEC_GAURAPURNIMA, "[c0]");
//			t.nFastType = FAST_MOONRISE;
		}
	}

	// test for Jagannatha Misra festival
	if (m_pData[nIndex-2].astrodata.nMasa == GOVINDA_MASA)
	{
		if (IsFestivalDay(m_pData[nIndex - 2], s, TITHI_PURNIMA))
		{
			AddSpecFestival(t, SPEC_MISRAFESTIVAL, "[c1]");
		}
	}


	// ------------------------
	// test for other festivals
	// ------------------------

	int n, n2;
	bool s1 = false, s2 = false;

	if (t.astrodata.nMasa > 11)
		goto other_fest;

	n = t.astrodata.nMasa * 30 + t.astrodata.nTithi;
	n2 = (n + 359 ) % 360; // this is index into table of festivals for previous tithi

	if (gstr[200 + n].IsEmpty() == false)
	{
		s1 = true;
	}

	// test for ksaya tithi at this date
	if ((t.astrodata.nTithi != s.astrodata.nTithi) && 
		(t.astrodata.nTithi != (s.astrodata.nTithi + 1)%30))
	{
		// we have to add also festivals which was
		// tithi before
		if (gstr[200 + n2].IsEmpty() == false)
		{
			s2 = true;
		}
	}

	// if this is second day of vriddhi, then No festivals
	if (s.astrodata.nTithi == t.astrodata.nTithi)
		s1 = false;

	if (s2)
	{
		if (t.festivals.IsEmpty() == false)
			t.festivals += "#";
		t.festivals += gstr[200 + n2];
	}

	if (s1)
	{
		if (t.festivals.IsEmpty() == false)
			t.festivals += "#";
		t.festivals += gstr[200 + n];
	}

	//-----------------------------------
	// custom events
/*	if (gCustomEventTexts[n].IsEmpty() == false)
	{
		s1 = true;
	}*/

	// test for ksaya tithi at this date
	if ((t.astrodata.nTithi != s.astrodata.nTithi) && 
		(t.astrodata.nTithi != (s.astrodata.nTithi + 1)%30))
	{
		// we have to add also festivals which was
		// tithi before
/*		if (gCustomEventTexts[n2].IsEmpty() == false)
		{
			s2 = true;
		}*/
	}

	// if this is second day of vriddhi, then No festivals
	if (s.astrodata.nTithi == t.astrodata.nTithi)
		s1 = false;

/*	if (s2)
	{
		if (t.festivals.IsEmpty() == false)
			t.festivals += "#";
		t.festivals += gCustomEventTexts[n2];
	}*/

/*	if (s1)
	{
		if (t.festivals.IsEmpty() == false)
			t.festivals += "#";
		t.festivals += gCustomEventTexts[n];
	}*/


other_fest:
	// ---------------------------
	// bhisma pancaka test
	// ---------------------------

	if (t.astrodata.nMasa == DAMODARA_MASA)
	{
		if ((t.astrodata.nPaksa == GAURA_PAKSA) && (t.nFastType == FAST_EKADASI))
		{
			if (t.festivals.GetLength() > 0)
				t.festivals += "#";
			t.festivals += gstr[81];
		}
	}

	// ---------------------------
	// caturmasya tests
	// ---------------------------
	
	// first month for punima and ekadasi systems
	if (t.astrodata.nMasa == VAMANA_MASA)
	{
		// purnima system
		if (TITHI_TRANSIT(s.astrodata.nTithi, t.astrodata.nTithi, TITHI_GAURA_CATURDASI, TITHI_PURNIMA))
		{
			t.nCaturmasya |= CMASYA_PURN_1_FIRST;
		}

		// ekadasi system
		//if (TITHI_TRANSIT(s.astrodata.nTithi, t.astrodata.nTithi, TITHI_GAURA_DASAMI, TITHI_GAURA_EKADASI))
		if ((t.astrodata.nPaksa == GAURA_PAKSA) && (t.nMhdType != EV_NULL))
		{
			t.nCaturmasya |= CMASYA_EKAD_1_FIRST;
		}
	}

	// first month for pratipat system
	// month transit for purnima and ekadasi systems
	if (t.astrodata.nMasa == SRIDHARA_MASA)
	{
		if (s.astrodata.nMasa == ADHIKA_MASA)
		{
			t.nCaturmasya = CMASYA_1_CONT;
		}

		// pratipat system
		if (TITHI_TRANSIT(s.astrodata.nTithi, t.astrodata.nTithi, TITHI_PURNIMA, TITHI_KRSNA_PRATIPAT))
		{
			t.nCaturmasya |= CMASYA_PRAT_1_FIRST;
		}

		// first day of particular month for PURNIMA system, when purnima is not KSAYA
		if (TITHI_TRANSIT(t.astrodata.nTithi, u.astrodata.nTithi, TITHI_GAURA_CATURDASI, TITHI_PURNIMA))
		{
			u.nCaturmasya |= CMASYA_PURN_2_FIRST;
			t.nCaturmasya |= CMASYA_PURN_1_LAST;
		}

		// ekadasi system
		//if (TITHI_TRANSIT(s.astrodata.nTithi, t.astrodata.nTithi, TITHI_GAURA_DASAMI, TITHI_GAURA_EKADASI))
		if ((t.astrodata.nPaksa == GAURA_PAKSA) && (t.nMhdType != EV_NULL))
		{
			t.nCaturmasya |= CMASYA_EKAD_2_FIRST;
			s.nCaturmasya |= CMASYA_EKAD_1_LAST;
		}
	}

	// second month for pratipat system
	// month transit for purnima and ekadasi systems
	if (t.astrodata.nMasa == HRSIKESA_MASA)
	{
		if (s.astrodata.nMasa == ADHIKA_MASA)
		{
			t.nCaturmasya = CMASYA_2_CONT;
		}

		// pratipat system
		if (TITHI_TRANSIT(s.astrodata.nTithi, t.astrodata.nTithi, TITHI_PURNIMA, TITHI_KRSNA_PRATIPAT))
//		if (s.astrodata.nMasa == SRIDHARA_MASA)
		{
			t.nCaturmasya |= CMASYA_PRAT_2_FIRST;
			s.nCaturmasya |= CMASYA_PRAT_1_LAST;
		}

		// first day of particular month for PURNIMA system, when purnima is not KSAYA
		if (TITHI_TRANSIT(t.astrodata.nTithi, u.astrodata.nTithi, TITHI_GAURA_CATURDASI, TITHI_PURNIMA))
		{
			u.nCaturmasya |= CMASYA_PURN_3_FIRST;
			t.nCaturmasya |= CMASYA_PURN_2_LAST;
		}
		// ekadasi system
		if ((t.astrodata.nPaksa == GAURA_PAKSA) && (t.nMhdType != EV_NULL))
		//if (TITHI_TRANSIT(s.astrodata.nTithi, t.astrodata.nTithi, TITHI_GAURA_DASAMI, TITHI_GAURA_EKADASI))
		{
			t.nCaturmasya |= CMASYA_EKAD_3_FIRST;
			s.nCaturmasya |= CMASYA_EKAD_2_LAST;
		}
	}

	// third month for pratipat
	// month transit for purnima and ekadasi systems
	if (t.astrodata.nMasa == PADMANABHA_MASA)
	{
		if (s.astrodata.nMasa == ADHIKA_MASA)
		{
			t.nCaturmasya = CMASYA_3_CONT;
		}
		// pratipat system
		if (TITHI_TRANSIT(s.astrodata.nTithi, t.astrodata.nTithi, TITHI_PURNIMA, TITHI_KRSNA_PRATIPAT))
//		if (s.astrodata.nMasa == HRSIKESA_MASA)
		{
			t.nCaturmasya |= CMASYA_PRAT_3_FIRST;
			s.nCaturmasya |= CMASYA_PRAT_2_LAST;
		}

		// first day of particular month for PURNIMA system, when purnima is not KSAYA
		if (TITHI_TRANSIT(t.astrodata.nTithi, u.astrodata.nTithi, TITHI_GAURA_CATURDASI, TITHI_PURNIMA))
		{
			u.nCaturmasya |= CMASYA_PURN_4_FIRST;
			t.nCaturmasya |= CMASYA_PURN_3_LAST;
		}

		// ekadasi system
		if ((t.astrodata.nPaksa == GAURA_PAKSA) && (t.nMhdType != EV_NULL))
		//if (TITHI_TRANSIT(s.astrodata.nTithi, t.astrodata.nTithi, TITHI_GAURA_DASAMI, TITHI_GAURA_EKADASI))
		{
			t.nCaturmasya |= CMASYA_EKAD_4_FIRST;
			s.nCaturmasya |= CMASYA_EKAD_3_LAST;
		}
	}

	// fourth month for pratipat system
	// month transit for purnima and ekadasi systems
	if (t.astrodata.nMasa == DAMODARA_MASA)
	{
		if (s.astrodata.nMasa == ADHIKA_MASA)
		{
			t.nCaturmasya = CMASYA_4_CONT;
		}
		// pratipat system
		if (TITHI_TRANSIT(s.astrodata.nTithi, t.astrodata.nTithi, TITHI_PURNIMA, TITHI_KRSNA_PRATIPAT))
		{
			t.nCaturmasya |= CMASYA_PRAT_4_FIRST;
			s.nCaturmasya |= CMASYA_PRAT_3_LAST;
		}

		// last day for punima system
		if (TITHI_TRANSIT(t.astrodata.nTithi, u.astrodata.nTithi, TITHI_GAURA_CATURDASI, TITHI_PURNIMA))
		{
			t.nCaturmasya |= CMASYA_PURN_4_LAST;
		}

		// ekadasi system
		//if (TITHI_TRANSIT(t.astrodata.nTithi, u.astrodata.nTithi, TITHI_GAURA_DASAMI, TITHI_GAURA_EKADASI))
		if ((t.astrodata.nPaksa == GAURA_PAKSA) && (t.nMhdType != EV_NULL))
		{
			s.nCaturmasya |= CMASYA_EKAD_4_LAST;
		}

		if (TITHI_TRANSIT(t.astrodata.nTithi, u.astrodata.nTithi, TITHI_PURNIMA, TITHI_KRSNA_PRATIPAT))
		{
			t.nCaturmasya |= CMASYA_PRAT_4_LAST;

			// on last day of Caturmasya pratipat system is Bhisma Pancaka ending
			if (t.festivals.GetLength() > 0)
				t.festivals += "#";
			t.festivals += gstr[82];
		}
	}

	// vaisnava calendar calculations are valid
	t.fVaisValid = true;

	return 1;
}


/******************************************************************************************/
/*                                                                                        */
/*                                                                                        */
/*                                                                                        */
/******************************************************************************************/

int TResultCalendar::MahadvadasiCalc(int nIndex, EARTHDATA earth)
{
	int nMahaType = NULL;
	int nMhdDay = -1;

	VAISNAVADAY & s = m_pData[nIndex-1];
	VAISNAVADAY & t = m_pData[nIndex];
	VAISNAVADAY & u = m_pData[nIndex+1];

	// if yesterday is dvadasi
	// then we skip this day
	if (TITHI_DVADASI(s.astrodata.nTithi))
		return 1;

	if (TITHI_GAURA_DVADASI == t.astrodata.nTithi && TITHI_GAURA_DVADASI == t.astrodata.nTithiSunset && IsMhd58(nIndex, nMahaType))
	{
		t.nMhdType = nMahaType;
		nMhdDay = nIndex;
	}
	else if (TITHI_DVADASI(t.astrodata.nTithi))
	{
		if (TITHI_DVADASI(u.astrodata.nTithi) && TITHI_EKADASI(s.astrodata.nTithi) && TITHI_EKADASI(s.astrodata.nTithiArunodaya))
		{
			t.nMhdType = EV_VYANJULI;
			nMhdDay = nIndex;
		}
		else if (NextNewFullIsVriddhi(nIndex, earth))
		{
			t.nMhdType = EV_PAKSAVARDHINI;
			nMhdDay = nIndex;
		}
		else if (TITHI_LESS_EKADASI(s.astrodata.nTithiArunodaya))
		{
			t.nMhdType = EV_SUDDHA;
			nMhdDay = nIndex;
		}
	}

	if (nMhdDay >= 0)
	{
		// fasting day
		m_pData[nMhdDay].nFastType = FAST_EKADASI;
		m_pData[nMhdDay].ekadasi_vrata_name = GetEkadasiName(t.astrodata.nMasa, t.astrodata.nPaksa);
		m_pData[nMhdDay].ekadasi_parana = false;
		m_pData[nMhdDay].eparana_time1 = 0.0;
		m_pData[nMhdDay].eparana_time2 = 0.0;
		m_pData[nMhdDay].fVaisValid = true;

		// parana day
		m_pData[nMhdDay + 1].nFastType = FAST_NULL;
		m_pData[nMhdDay + 1].ekadasi_parana = true;
		m_pData[nMhdDay + 1].eparana_time1 = 0.0;
		m_pData[nMhdDay + 1].eparana_time2 = 0.0;
		m_pData[nMhdDay + 1].fVaisValid = true;
	}

	return 1;
}

VAISNAVADAY * TResultCalendar::GetDay(int nIndex)
{
	int nReturn = nIndex + BEFORE_DAYS;

	if (nReturn >= m_nCount)
		return NULL;

	return &m_pData[nReturn];
}

/******************************************************************************************/
/*                                                                                        */
/*                                                                                        */
/*                                                                                        */
/******************************************************************************************/

int TResultCalendar::ExtendedCalc(int nIndex, EARTHDATA earth)
{
	VAISNAVADAY & s = m_pData[nIndex-1];
	VAISNAVADAY & t = m_pData[nIndex];
	VAISNAVADAY & u = m_pData[nIndex+1];
	VAISNAVADAY & v = m_pData[nIndex+2];

	// test for Rama Navami
	if ((t.astrodata.nMasa == VISNU_MASA) && (t.astrodata.nPaksa == GAURA_PAKSA))
	{
		if (IsFestivalDay(s, t, TITHI_GAURA_NAVAMI))
		{
			if (u.nFastType >= FAST_EKADASI)
			{
				// yesterday was Rama Navami
				AddSpecFestival(s, SPEC_RAMANAVAMI, "[c0]");
				//s.nFastType = FAST_SUNSET;
			}
			else
			{
				// today is Rama Navami
				AddSpecFestival(t, SPEC_RAMANAVAMI, "[c0]");
				//t.nFastType = FAST_SUNSET;
			}
		}
	}

	return 1;
}

/******************************************************************************************/
/*                                                                                        */
/*  TEST if today is given festival tithi                                                 */
/*                                                                                        */
/*  if today is given tithi and yesterday is not this tithi                               */
/*  then it is festival day (it is first day of this tithi, when vriddhi)                 */
/*                                                                                        */
/*  if yesterday is previous tithi to the given one and today is next to the given one    */
/*  then today is day after ksaya tithi which is given                                    */
/*                                                                                        */
/*                                                                                        */
/******************************************************************************************/

Boolean TResultCalendar::IsFestivalDay(VAISNAVADAY &yesterday, VAISNAVADAY &today, int nTithi)
{
	return ((today.astrodata.nTithi == nTithi) && TITHI_LESS_THAN(yesterday.astrodata.nTithi, nTithi))
			|| (TITHI_LESS_THAN(yesterday.astrodata.nTithi, nTithi) && TITHI_GREAT_THAN(today.astrodata.nTithi, nTithi));

}

int TResultCalendar::FindDate(VCTIME vc)
{
	int i;
	for(i = BEFORE_DAYS; i < m_nCount; i++)
	{
		if ((m_pData[i].date.day == vc.day) && (m_pData[i].date.month == vc.month) && (m_pData[i].date.year == vc.year))
			return (i - BEFORE_DAYS);
	}

	return -1;
}

double GcGetHigher(double a, double b)
{
	if (a > b)
		return a;
	return b;
}

double GcGetLower(double a, double b)
{
	if (a < b)
		return a;
	return b;
}

double GcGetNaksatraEndHour(EARTHDATA earth, VCTIME yesterday, VCTIME today)
{
	VCTIME nend;
	VCTIME snd = yesterday;
	snd.shour = 0.5;
	GetNextNaksatra(earth, snd, nend);
	return nend.GetJulian() - today.GetJulian() + nend.shour;
}

int TResultCalendar::CalculateEParana(VAISNAVADAY &s, VAISNAVADAY &t, double &begin, double &end, EARTHDATA earth)
{
	t.nMhdType = EV_NULL;
	t.ekadasi_parana = true;
	t.nFastType = FAST_NULL;

	double titBeg, titEnd, tithi_quart;
	double sunRise, third_day, naksEnd;
	double parBeg = -1.0, parEnd = -1.0;
	double tithi_len;
	VCTIME snd, nend;

	sunRise = t.astrodata.sun.sunrise_deg / 360.0 + earth.tzone / 24.0;
	third_day = sunRise + t.astrodata.sun.length_deg / 1080.0;
	tithi_len = GetTithiTimes(earth, t.date, titBeg, titEnd, sunRise);
	tithi_quart = tithi_len / 4.0 + titBeg;

	switch(s.nMhdType)
	{
	case EV_UNMILANI:
		parEnd = titEnd;
		if (parEnd > third_day)
			parEnd = third_day;
		parBeg = sunRise;
		break;
	case EV_VYANJULI:
		parBeg = sunRise;
		parEnd = GcGetLower(titEnd, third_day);
		break;
	case EV_TRISPRSA:
		parBeg = sunRise;
		parEnd = third_day;
		break;
	case EV_JAYANTI:
	case EV_VIJAYA:

		naksEnd = GcGetNaksatraEndHour(earth, s.date, t.date); //GetNextNaksatra(earth, snd, nend);
		if (TITHI_DVADASI(t.astrodata.nTithi))
		{
			if (naksEnd < titEnd)
			{
				if (naksEnd < third_day)
				{
					parBeg = naksEnd;
					parEnd = GcGetLower(titEnd, third_day);
				}
				else
				{
					parBeg = naksEnd;
					parEnd = titEnd;
				}
			}
			else
			{
				parBeg = sunRise;
				parEnd = GcGetLower(titEnd, third_day);
			}
		}
		else
		{
			parBeg = sunRise;
			parEnd = GcGetLower( naksEnd, third_day );
		}

		break;
	case EV_JAYA:
	case EV_PAPA_NASINI:

		naksEnd = GcGetNaksatraEndHour(earth, s.date, t.date); //GetNextNaksatra(earth, snd, nend);

		if (TITHI_DVADASI(t.astrodata.nTithi))
		{
			if (naksEnd < titEnd)
			{
				if (naksEnd < third_day)
				{
					parBeg = naksEnd;
					parEnd = GcGetLower(titEnd, third_day);
				}
				else
				{
					parBeg = naksEnd;
					parEnd = titEnd;
				}
			}
			else
			{
				parBeg = sunRise;
				parEnd = GcGetLower(titEnd, third_day);
			}
		}
		else
		{
			if (naksEnd < third_day)
			{
				parBeg = naksEnd;
				parEnd = third_day;
			}
			else
			{
				parBeg = naksEnd;
				parEnd = -1.0;
			}
		}

		break;
	default:
		// first initial
		parEnd = GcGetLower(titEnd, third_day);
		parBeg = GcGetHigher(sunRise, tithi_quart);

		if (TITHI_DVADASI(s.astrodata.nTithi))
		{
			parBeg = sunRise;
		}

		//if (parBeg > third_day)
		if (parBeg > parEnd)
		{	
//			parBeg = sunRise;
			parEnd = -1.0;
		}
		break;
	}


	begin = parBeg;
	end = parEnd;

	if (begin > 0.0)
		begin *= 24.0;
	if (end > 0.0)
		end *= 24.0;

	t.eparana_time1 = begin;
	t.eparana_time2 = end;

	return 1;
}


/* Function before is writen accoring this algorithms:


1. Normal - fasting day has ekadasi at sunrise and dvadasi at next sunrise.

2. Viddha - fasting day has dvadasi at sunrise and trayodasi at next
sunrise, and it is not a naksatra mahadvadasi

3. Unmilani - fasting day has ekadasi at both sunrises

4. Vyanjuli - fasting day has dvadasi at both sunrises, and it is not a
naksatra mahadvadasi

5. Trisprsa - fasting day has ekadasi at sunrise and trayodasi at next
sunrise.

6. Jayanti/Vijaya - fasting day has gaura dvadasi and specified naksatra at
sunrise and same naksatra at next sunrise

7. Jaya/Papanasini - fasting day has gaura dvadasi and specified naksatra at
sunrise and same naksatra at next sunrise

==============================================
Case 1 Normal (no change)

If dvadasi tithi ends before 1/3 of daylight
   then PARANA END = TIME OF END OF TITHI
but if dvadasi TITHI ends after 1/3 of daylight
   then PARANA END = TIME OF 1/3 OF DAYLIGHT

if 1/4 of dvadasi tithi is before sunrise
   then PARANA BEGIN is sunrise time
but if 1/4 of dvadasi tithi is after sunrise
   then PARANA BEGIN is time of 1/4 of dvadasi tithi

if PARANA BEGIN is before PARANA END
   then we will write "BREAK FAST FROM xx TO yy
but if PARANA BEGIN is after PARANA END
   then we will write "BREAK FAST AFTER xx"

==============================================
Case 2 Viddha

If trayodasi tithi ends before 1/3 of daylight
   then PARANA END = TIME OF END OF TITHI
but if trayodasi TITHI ends after 1/3 of daylight
   then PARANA END = TIME OF 1/3 OF DAYLIGHT

PARANA BEGIN is sunrise time

we will write "BREAK FAST FROM xx TO yy

==============================================
Case 3 Unmilani

PARANA END = TIME OF 1/3 OF DAYLIGHT

PARANA BEGIN is end of Ekadasi tithi

if PARANA BEGIN is before PARANA END
   then we will write "BREAK FAST FROM xx TO yy
but if PARANA BEGIN is after PARANA END
   then we will write "BREAK FAST AFTER xx"

==============================================
Case 4 Vyanjuli

PARANA BEGIN = Sunrise

PARANA END is end of Dvadasi tithi

we will write "BREAK FAST FROM xx TO yy

==============================================
Case 5 Trisprsa

PARANA BEGIN = Sunrise

PARANA END = 1/3 of daylight hours

we will write "BREAK FAST FROM xx TO yy

==============================================
Case 6 Jayanti/Vijaya

PARANA BEGIN = Sunrise

PARANA END1 = end of dvadasi tithi or sunrise, whichever is later
PARANA END2 = end of naksatra

PARANA END is earlier of END1 and END2

we will write "BREAK FAST FROM xx TO yy

==============================================
Case 7 Jaya/Papanasini

PARANA BEGIN = end of naksatra

PARANA END = 1/3 of Daylight hours

if PARANA BEGIN is before PARANA END
   then we will write "BREAK FAST FROM xx TO yy
but if PARANA BEGIN is after PARANA END
   then we will write "BREAK FAST AFTER xx"



  */

Boolean TResultCalendar::AddSpecFestival(VAISNAVADAY &day, int nSpecialFestival, const char * pszClass)
{
	TString str;

	if (GetSpecFestivalNameEx(str, nSpecialFestival))
	{
		if (day.festivals.IsEmpty() == false)
		{
			day.festivals += "#";
			day.festivals += pszClass;
		}
		else
		{
			day.festivals = pszClass;
		}
		day.festivals += str;

		return true;
	}

	return false;
}

void TResultCalendar::ResolveFestivalsFasting(int nIndex)
{
	VAISNAVADAY & s = m_pData[nIndex-1];
	VAISNAVADAY & t = m_pData[nIndex];
	VAISNAVADAY & u = m_pData[nIndex+1];

	int nf, nf2, nftype;
	TString pers, str, S;
	int fasting = 0;
	const char * ch;

	if (t.nMhdType != EV_NULL)
	{
		str.Format("%s %s", gstr[87].c_str(), t.ekadasi_vrata_name.c_str());
		if (t.festivals.IsEmpty() == false)
		{
			str += "#";
			str += t.festivals;
		}
		t.festivals = str;
	}

	ch = GetMahadvadasiName(t.nMhdType);
	if (ch)
	{
		str = ch;
		if (t.festivals.IsEmpty() == false)
		{
			str += "#";
			str += t.festivals;
		}
		t.festivals = str;
	}

	// analyze for fasting
	nf = t.festivals.Find("[f:");
	while (nf >= 0 && nf < t.festivals.GetLength())
	{
		// ziskava typ postu
		nftype = ctoi(t.festivals.GetAt(nf + 3));
		nf2 = t.festivals.Find("]", nf);
		if (nf2 >= 0)
		{
			t.festivals.Mid(nf + 5, nf2, pers);
		}
		else
		{
			pers.Empty();
			nf2 = t.festivals.GetLength() - 1;
		}

		// zaraduje post do dni
		if (nftype)
		{
			if (s.nFastType == FAST_EKADASI)
			{
				if (s.festivals.IsEmpty() == false)
					s.festivals += "#";
				s.festivals += "(Fast till noon for ";
				s.festivals += pers;
				s.festivals += ", with feast tomorrow)";
				pers = "#(Fasting is done yesterday, today is feast)";
				t.festivals.Delete(nf, nf2 - nf + 1);
				t.festivals.Insert(nf, pers);
			}
			else if (t.nFastType == FAST_EKADASI)
			{
				pers = "#(Fasting till noon, with feast tomorrow)";
				t.festivals.Delete(nf, nf2 - nf + 1);
				t.festivals.Insert(nf, pers);
			}
			else
			{
				pers = "#";
				pers += GetFastingName(0x200 + nftype);
				t.festivals.Delete(nf, nf2 - nf + 1);
				t.festivals.Insert(nf, pers);
			}
		}
		if (fasting < nftype)
			fasting = nftype;

		nf = t.festivals.Find("[f:", nf + 1);
	}

	if (fasting)
	{
		if (s.nFastType == FAST_EKADASI)
		{
			t.nFeasting = FEAST_TODAY_FAST_YESTERDAY;
			s.nFeasting = FEAST_TOMMOROW_FAST_TODAY;
		}
		else if (t.nFastType == FAST_EKADASI)
		{
			u.nFeasting = FEAST_TODAY_FAST_YESTERDAY;
			t.nFeasting = FEAST_TOMMOROW_FAST_TODAY;
		}
		else
		{
			t.nFastType = 0x200 + fasting;
		}
	}

}
