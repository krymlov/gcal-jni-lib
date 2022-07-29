// PORTABLE
#ifndef _VCAL_SE_LEVEL_1
#define _VCAL_SE_LEVEL_1

#pragma GCC diagnostic ignored "-Wreturn-type"

#include "math.h"
#include "gmath.h"
#include "platform.h"
#include "tstring.h"

typedef double JULIANDATE;

typedef double DAYHOURS;

class VATIME
{
public:
	VATIME() { };
	VATIME(int t, int m, int y) { tithi = t; masa = m; gyear = y; }
	int tithi;
	int masa;
	int gyear;
	void next()
	{
		tithi++;
		if (tithi >= 30)
		{
			tithi %= 30;
			masa++;
		}
		if (masa >= 12)
		{
			masa %= 12;
			gyear++;
		}
	}
	void prev()
	{
		if (tithi == 0)
		{
			if (masa == 0)
			{
				masa = 11;
				tithi = 29;
			}
			else
			{
				masa--;
				tithi = 29;
			}
		}
		else
		{
			tithi--;
		}
	}
};

class VCTIME
{
public:
	const char * c_str();
	int GetSecond();
	int GetDayInteger();
	int GetMinute();
	int GetMinuteRound();
	int GetHour();
	int CompareYMD(VCTIME v);
	void Today();
	bool IsBeforeThis(VCTIME &date);
	int operator=(int i);
	void SetFromJulian(JULIANDATE jd);
	void InitWeekDay(void);
	double GetJulianComplete();
	double GetJulianDetailed();
	void operator+=(int);
	void operator-=(int);
	void NextDay();
	void PreviousDay();
	double GetJulian();
	void ChangeTimeZone(double);
	bool IsLeapYear();
	bool IsLeapYear(int year);

	int GetMonthMaxDays(int year, int month);

	void NormalizeValues();
	void NormalizeValues(int &y, int &m, int &d, double &h);
	bool operator<(VCTIME &date);
	bool operator<=(VCTIME &date);
	bool operator==(VCTIME &date);
    int year;
    int month;
    int dayOfWeek;
    int day;
	DAYHOURS shour;
	double tzone;
	VCTIME() { 
		year = 0;
		month = 0;
		dayOfWeek = 0;
		day = 0;
		shour = 0.0;
		tzone = 0.0;
	}
	
};

class DAYTIME
{
public:
	double GetDayTime(void);
	Boolean operator<(DAYTIME &dt);
	Boolean operator>(DAYTIME &dt);
	Boolean operator>=(DAYTIME &dt);
	Boolean operator<=(DAYTIME &dt);
	void SetValue(int i);
	void SetDayTime(double d);
	int hour;
	int min;
	int sec;
	int mili;
	void SetDegTime(double);
};

class MOONDATA 
{
public:
	/*// illumination (synodic) phase
	double ip;
	// age of moon
	int age;
	// distance from anomalistic phase
	double di;*/
	// latitude from nodal (draconic) phase
	double latitude_deg;
	// longitude from sidereal motion
	double longitude_deg;

	double radius; //(* lambda, beta, R *)
	double rektaszension, declination;  //(* alpha, delta *)
	double parallax;
	double elevation, azimuth;//          (* h, A *)

};

class EARTHDATA
{
public:
	int dst;
	// terrestrial longitude of observation
	double longitude_deg;

	// terrestrial latitude of observation
	double latitude_deg;

	// time zone (hours)
	double tzone;

	// observated event
	// 0 - center of the sun
	// 1 - civil twilight
	// 2 - nautical twilight
	// 3 - astronomical twilight
	int obs;

	EARTHDATA() { obs = 0; longitude_deg = 0.0; latitude_deg = 0.0; tzone = 0.0; dst = 0;}
};

class SUNDATA
{
public:
	double length_deg;
	double arunodaya_deg;
	double sunrise_deg;
	double sunset_deg;

	double declination_deg;
	double longitude_deg;
	double longitude_set_deg;
	double longitude_arun_deg;
	double right_asc_deg;

	// time of arunodaya - 96 mins before sunrise
	DAYTIME arunodaya;
	// time of sunrise
	DAYTIME rise;
	// time of noon
	DAYTIME noon;
	// time of sunset
	DAYTIME set;
	// length of the day
	DAYTIME length;
};

class DAYDATA
{
public:
	// date of Julian epoch
	JULIANDATE jdate;
	// sun
	SUNDATA sun;
	// moon
	MOONDATA moon;
	// year of Gaurabda epoch
	int nGaurabdaYear;
	// value of ayanamsa for this date
	double msAyanamsa;
	// sign of zodiac
	int nRasi;
	// tithi #
	int nTithi;
	// tithi at arunodaya
	int nTithiArunodaya;
	// tithi at sunset
	int nTithiSunset;
	// tithi elaps.
	double nTithiElapse;
	// paksa
	int nPaksa;
	// yoga
	int nYoga;
	// yoga elaps.
	double nYogaElapse;
	// naksatra
	int nNaksatra;
	// naksatra elaps.
	double nNaksatraElapse;
	// masa
	int nMasa;
	// distance of moon and sun in degrees
	double msDistance;
};

#define EV_NULL              0x100
#define EV_SUDDHA            0x101
#define EV_UNMILANI          0x102
#define EV_VYANJULI          0x103
#define EV_TRISPRSA          0x104
#define EV_UNMILANI_TRISPRSA 0x105
#define EV_PAKSAVARDHINI     0x106
#define EV_JAYA              0x107
#define EV_JAYANTI           0x108
#define EV_PAPA_NASINI       0x109
#define EV_VIJAYA            0x110

#define FAST_NULL      0x0
#define FAST_NOON      0x201
#define FAST_SUNSET    0x202
#define FAST_MOONRISE  0x203
#define FAST_DUSK      0x204
#define FAST_MIDNIGHT  0x205
#define FAST_EKADASI   0x206

#define FEAST_NULL     0
#define FEAST_TODAY_FAST_YESTERDAY    1
#define FEAST_TOMMOROW_FAST_TODAY     2

#define SPEC_RETURNRATHA    3
#define SPEC_HERAPANCAMI    4
#define SPEC_GUNDICAMARJANA 5
#define SPEC_GOVARDHANPUJA  6
#define SPEC_VAMANADVADASI  7
#define SPEC_VARAHADVADASI  8
#define SPEC_RAMANAVAMI     9
#define SPEC_JANMASTAMI    10
#define SPEC_RATHAYATRA    11
#define SPEC_GAURAPURNIMA  12
#define SPEC_NANDAUTSAVA   13
#define SPEC_MISRAFESTIVAL 14
#define SPEC_PRABHAPP      15


#define CMASYA_PURN_1_FIRST 0x000011
#define CMASYA_PURN_1_LAST  0x000012
#define CMASYA_PURN_2_FIRST 0x000021
#define CMASYA_PURN_2_LAST  0x000022
#define CMASYA_PURN_3_FIRST 0x000031
#define CMASYA_PURN_3_LAST  0x000032
#define CMASYA_PURN_4_FIRST 0x000041
#define CMASYA_PURN_4_LAST  0x000042

#define CMASYA_PRAT_1_FIRST 0x001100
#define CMASYA_PRAT_1_LAST  0x001200
#define CMASYA_PRAT_2_FIRST 0x002100
#define CMASYA_PRAT_2_LAST  0x002200
#define CMASYA_PRAT_3_FIRST 0x003100
#define CMASYA_PRAT_3_LAST  0x003200
#define CMASYA_PRAT_4_FIRST 0x004100
#define CMASYA_PRAT_4_LAST  0x004200

#define CMASYA_EKAD_1_FIRST 0x110000
#define CMASYA_EKAD_1_LAST  0x120000
#define CMASYA_EKAD_2_FIRST 0x210000
#define CMASYA_EKAD_2_LAST  0x220000
#define CMASYA_EKAD_3_FIRST 0x310000
#define CMASYA_EKAD_3_LAST  0x320000
#define CMASYA_EKAD_4_FIRST 0x410000
#define CMASYA_EKAD_4_LAST  0x420000

#define CMASYA_1_CONT       0x01000000
#define CMASYA_2_CONT       0x02000000
#define CMASYA_3_CONT       0x03000000
#define CMASYA_4_CONT       0x04000000
#define CMASYA_CONT_MASK    0x0f000000


#define CMASYA_PURN_MASK_MASA    0x0000f0
#define CMASYA_PURN_MASK_DAY     0x00000f
#define CMASYA_PURN_MASK         0x0000ff
#define CMASYA_PRAT_MASK_MASA    0x00f000
#define CMASYA_PRAT_MASK_DAY     0x000f00
#define CMASYA_PRAT_MASK         0x00ff00
#define CMASYA_EKAD_MASK         0xff0000
#define CMASYA_EKAD_MASK_MASA    0xf00000
#define CMASYA_EKAD_MASK_DAY     0x0f0000

class VAISNAVADAY
{
public:
	int GetLineCount();
	int GetFestivalClass(TString &str);
	Boolean GetTithiTimeRange(EARTHDATA earth, VCTIME &from, VCTIME &to);
	Boolean GetNaksatraTimeRange(EARTHDATA earth, VCTIME &from, VCTIME &to);
	Boolean GetNextFestival(int &i, TString &str);
	int GetHeadFestival(void);
	UInt32 nCaturmasya;
	int nDST;
	int ksaya_day2;
	int ksaya_day1;
//	int nSpecFestival;
	int nFeasting;
	void Clear();
	// date
	VCTIME date;
	// moon times
	DAYTIME moonrise;
	DAYTIME moonset;
	// astronomical data from astro-sub-layer
	DAYDATA astrodata;
	// data for vaisnava calculations
	TString festivals;
	int nFastType;
	int nMhdType;
	TString ekadasi_vrata_name;
	bool ekadasi_parana;
	double eparana_time1, eparana_time2;
	int sankranti_zodiac;
	//double sankranti_time;
	VCTIME sankranti_day;
	bool was_ksaya;
	double ksaya_time1, ksaya_time2;
	bool is_vriddhi;
	// flag for validity
	bool fDateValid;
	bool fAstroValid;
	bool fVaisValid;
	VAISNAVADAY() {
		fDateValid = false;
		fAstroValid = false;
		fVaisValid = false;
		// init
		//festivals = "";
		nFastType = FAST_NULL;
		nMhdType = EV_NULL;
		ekadasi_parana = false;
		ekadasi_vrata_name.Empty();
		eparana_time1 = eparana_time2 = 0.0;
		sankranti_zodiac = -1;
		was_ksaya = false;
		ksaya_time1 = ksaya_time2 = 0.0;
		is_vriddhi = false;
		nDST = 0;
		nCaturmasya = 0;
		moonrise.SetValue(0);
		moonset.SetValue(0);
	}
	~VAISNAVADAY()
	{
		/*		if (festivals)
		{
			delete [] festivals;
		}*/
	}
	int operator=(VCTIME &vc){ date = vc; fDateValid = true; }
	int operator=(DAYDATA &dd) { astrodata = dd; fAstroValid = true; }
};

double GetJulianDay(VCTIME &);

double GetJulianDay(int, int, int);

int GetGregorianYearLength(bool bOverlapped);

int GetGregorianDay(bool bOverlapped, int nDayIndex);

int GetGregorianMonth(bool bOverlapped, int nDayIndex);

void GetNextDay(VCTIME &day);
void GetPrevDay(VCTIME &day);
int  GetMonthMaxDays(int year, int month);
bool IsLeapYear(int year);

char * AvcGetTextLatitude(double d);
char * AvcGetTextLongitude(double d);
char * AvcGetTextTimeZone(double d);
const char * AvcGetMonthAbr(int n);
void AvcGetDateText(TString &str, VCTIME vc);
int GetAyanamsaType();
int SetAyanamsaType(int i);
const char * GetAyanamsaName(int nType);

//double GetSunLongitude(JULIANDATE jDate);

double GetSunLongitude(VCTIME date);

void SunPosition(VCTIME vct, EARTHDATA ed, SUNDATA &sun, DAYHOURS DayHours);

void SunCalc(VCTIME, EARTHDATA, SUNDATA &);

double GetAyanamsa(JULIANDATE);

int GetRasi(double SunLongitude, double Ayanamsa);

VCTIME GetNextSankranti( VCTIME startDate, int &zodiac);

void MoonCalc(JULIANDATE, MOONDATA &, EARTHDATA);

void calc_horizontal(MOONDATA &, JULIANDATE, double, double);


#define DCEX_NAKSATRA_MIDNIGHT 3
#define DCEX_MOONRISE          4
#define DCEX_SUNRISE           5

DAYHOURS GetTithiTimes(EARTHDATA, VCTIME vc, DAYHOURS &titBeg, DAYHOURS &titEnd, DAYHOURS sunRise);

double GetPrevConjunction(VCTIME, VCTIME &, bool, EARTHDATA);
double GetNextConjunction(VCTIME, VCTIME &, bool, EARTHDATA);


int DayCalc(VCTIME date, EARTHDATA earth, DAYDATA &day);
int MasaCalc(VCTIME date, DAYDATA day, EARTHDATA earth,int&);
VCTIME GetFirstDayOfYear(EARTHDATA,int);
VCTIME GetFirstDayOfMasa(EARTHDATA earth, int GYear, int nMasa);
int GetNextNaksatra(EARTHDATA ed, VCTIME startDate, VCTIME &nextDate);
int GetPrevNaksatra(EARTHDATA ed, VCTIME startDate, VCTIME &prevDate);
int GetPrevTithiStart(EARTHDATA ed, VCTIME startDate, VCTIME &nextDate);
int GetNextTithiStart(EARTHDATA ed, VCTIME startDate, VCTIME &nextDate);
double DayCalcEx(VCTIME date, EARTHDATA earth, int nType);

#define MADHUSUDANA_MASA 0
#define TRIVIKRAMA_MASA  1
#define VAMANA_MASA      2
#define SRIDHARA_MASA    3
#define HRSIKESA_MASA    4
#define PADMANABHA_MASA  5
#define DAMODARA_MASA    6
#define KESAVA_MASA      7
#define NARAYANA_MASA    8
#define MADHAVA_MASA     9
#define GOVINDA_MASA     10
#define VISNU_MASA       11
#define ADHIKA_MASA      12

#define MESHA_SANKRANTI   0
#define VRSABHA_SANKRANTI 1
#define MITHUNA_SANKRANTI 2
#define KATAKA_SANKRANTI  3
#define SIMHA_SANKRANTI   4
#define KANYA_SANKRANTI   5
#define TULA_SANKRANTI    6
#define VRSCIKA_SANKRANTI 7
#define DHANUS_SANKRANTI  8
#define MAKARA_SANKRANTI  9
#define KUMBHA_SANKRANTI  10
#define MINA_SANKRANTI    11


#define KRSNA_PAKSA            0
#define GAURA_PAKSA            1

#define TITHI_KRSNA_PRATIPAT   0
#define TITHI_KRSNA_DVITIYA    1
#define TITHI_KRSNA_TRITIYA    2
#define TITHI_KRSNA_CATURTI    3
#define TITHI_KRSNA_PANCAMI    4
#define TITHI_KRSNA_SASTI      5
#define TITHI_KRSNA_SAPTAMI    6
#define TITHI_KRSNA_ASTAMI     7
#define TITHI_KRSNA_NAVAMI     8
#define TITHI_KRSNA_DASAMI     9
#define TITHI_KRSNA_EKADASI   10
#define TITHI_KRSNA_DVADASI   11
#define TITHI_KRSNA_TRAYODASI 12
#define TITHI_KRSNA_CATURDASI 13
#define TITHI_AMAVASYA        14
#define TITHI_GAURA_PRATIPAT  15
#define TITHI_GAURA_DVITIYA   16
#define TITHI_GAURA_TRITIYA   17
#define TITHI_GAURA_CATURTI   18
#define TITHI_GAURA_PANCAMI   19
#define TITHI_GAURA_SASTI     20
#define TITHI_GAURA_SAPTAMI   21
#define TITHI_GAURA_ASTAMI    22
#define TITHI_GAURA_NAVAMI    23
#define TITHI_GAURA_DASAMI    24
#define TITHI_GAURA_EKADASI   25
#define TITHI_GAURA_DVADASI   26
#define TITHI_GAURA_TRAYODASI 27
#define TITHI_GAURA_CATURDASI 28
#define TITHI_PURNIMA         29

#define ROHINI_NAKSATRA       3

#define DW_SUNDAY    0
#define DW_MONDAY    1
#define DW_TUESDAY   2
#define DW_WEDNESDAY 3
#define DW_THURSDAY  4
#define DW_FRIDAY    5
#define DW_SATURDAY  6

#define TITHI_EKADASI(a) (((a) == 10) || ((a) == 25))
#define TITHI_DVADASI(a) (((a) == 11) || ((a) == 26))
#define TITHI_TRAYODASI(a) (((a) == 12) || ((a) == 27))
#define TITHI_CATURDASI(a) (((a) == 13) || ((a) == 28))

#define TITHI_LESS_EKADASI(a) (((a) == 9) || ((a) == 24) || ((a) == 8) || ((a) == 23))
#define TITHI_LESS_DVADASI(a) (((a) == 10) || ((a) == 25) || ((a) == 9) || ((a) == 24))
#define TITHI_LESS_TRAYODASI(a) (((a) == 11) || ((a) == 26) || ((a) == 10) || ((a) == 25))
#define TITHI_FULLNEW_MOON(a) (((a) == 14) || ((a) == 29))

#define PREV_PREV_TITHI(a) (((a) + 28) % 30)
#define PREV_TITHI(a) (((a) + 29) % 30)
#define NEXT_TITHI(a) (((a) + 1) % 30)
#define NEXT_NEXT_TITHI(a) (((a) + 1) % 30)

#define TITHI_LESS_THAN(a,b) (((a) == PREV_TITHI(b)) || ((a) == PREV_PREV_TITHI(b)))
#define TITHI_GREAT_THAN(a,b) (((a) == NEXT_TITHI(b)) || ((a) == NEXT_NEXT_TITHI(b)))

// TRUE when transit from A to B is between T and U
#define TITHI_TRANSIT(t,u,a,b) (((t)==(a)) && ((u)==(b))) || (((t)==(a)) && ((u)==NEXT_TITHI(b))) || (((t)==PREV_TITHI(a)) && ((u) == (b)))



VCTIME CalcTithiEnd(int nGYear, int nMasa, int nPaksa, int nTithi, EARTHDATA, VCTIME &);
VCTIME CalcTithiEndEx(VCTIME vcStart, int GYear, int nMasa, int nPaksa, int nTithi, EARTHDATA earth, VCTIME & endTithi);
VCTIME CalcTithiDate(int nGYear, int nMasa, int nPaksa, int nTithi, EARTHDATA earth);
int	GetGaurabdaYear(VCTIME vc, EARTHDATA earth);
void VATIMEtoVCTIME(VATIME va, VCTIME &vc, EARTHDATA earth);
void VCTIMEtoVATIME(VCTIME vc, VATIME &va, EARTHDATA earth);


class CLocation  
{
public:
	const char * getFullName() 
	{
		m_strFullName = m_strCity;
		m_strFullName += " [";
		m_strFullName += m_strCountry;
		m_strFullName += "]    ";
		m_strFullName += AvcGetTextLatitude(m_fLatitude);
		m_strFullName += " ";
		m_strFullName += AvcGetTextLongitude(m_fLongitude);
		m_strFullName += " ";
		m_strFullName += AvcGetTextTimeZone(m_fTimezone);
		return m_strFullName.c_str();
	}
	void Empty()
	{
		m_strCountry.Empty();
		m_strCity.Empty();
		m_fLatitude = 0.0;
		m_fLongitude = 0.0;
		m_fTimezone = 0.0;
		m_nDST = 0;
	}
	operator EARTHDATA() {
		EARTHDATA e; 
		e.dst = m_nDST; 
		e.latitude_deg = m_fLatitude; 
		e.longitude_deg = m_fLongitude;
		e.tzone = m_fTimezone; return e;
	};
	EARTHDATA operator=(EARTHDATA e) { 
		m_fLongitude = e.longitude_deg; 
		m_fLatitude = e.latitude_deg; 
		m_fTimezone = e.tzone; 
		m_nDST = e.dst; return e;
	};
	TString m_strCountry;
	TString m_strCity;
	double m_fLongitude;
	double m_fLatitude;
	double m_fTimezone;
	int m_nDST;
//	TString m_strTimeZone;
//	TString m_strLatitude;
//	TString m_strLongitude;
//	TString m_strName;
private:
	TString m_strFullName;
public:
	CLocation * next;
	CLocation * prev;

	CLocation() {
		m_fLatitude = 0.0;
		m_fLongitude = 0.0;
		m_fTimezone = 0.0;
		m_nDST = 0;
		next = NULL;
		prev = NULL;
	}
	virtual ~CLocation() {};

};

#define CDB_MAXDAYS 16
#define BEFORE_DAYS 8

class TResultCalendar  
{
public:
	VAISNAVADAY * m_pData;
	int m_nCount;
	int m_PureCount;
	CLocation m_Location;
	VCTIME m_vcStart;
	int m_vcCount;
public:
	void ResolveFestivalsFasting(int i);
	Boolean AddSpecFestival(VAISNAVADAY &day, int nSpecialFestival, const char * pszClass);
	int CalculateEParana(VAISNAVADAY &s, VAISNAVADAY &t, double &begin, double &end, EARTHDATA earth);
	int FindDate(VCTIME vc);
	Boolean IsFestivalDay(VAISNAVADAY &yesterday, VAISNAVADAY &today, int nTithi);
	VAISNAVADAY * GetDay(int nIndex);
	int MahadvadasiCalc(int nIndex, EARTHDATA earth);
	int CompleteCalc(int nIndex, EARTHDATA earth);
	int EkadasiCalc(int nIndex, EARTHDATA earth);
	int ExtendedCalc(int nIndex, EARTHDATA earth);
	int CalculateCalendar(CLocation & loc, VCTIME begDate, int iCount);
	Boolean IsMhd58(int nIndex, int &nMahaType);
	Boolean NextNewFullIsVriddhi(int nIndex, EARTHDATA earth);
	int nBeg;
	int nTop;
	TResultCalendar();
	virtual ~TResultCalendar();

	int top;
	VAISNAVADAY days[CDB_MAXDAYS];

	int Push(VAISNAVADAY &day);
};


class TDayEvent
{
public:
	int nType;
	int nData;
	VCTIME Time;
};
// PORTABLE

typedef TDayEvent * TDayEventPtr;

class TResultEvents
{
public:
	VCTIME  m_vcStart;
	VCTIME  m_vcEnd;
	UInt32 m_options;
	CLocation m_location;
	void clear();
	TDayEventPtr * e_ptr;
	TDayEvent * p_events;
	UInt32      n_count;
	UInt32      n_size;
	Boolean     b_sorted;
	Boolean AddEvent(VCTIME inTime, int inType, int inData);
	void Sort(Boolean inSort);
	TDayEvent &operator[](int i)
	{
		if (b_sorted) return *(e_ptr[i]);
		else return p_events[i];
	}
	TResultEvents();
	~TResultEvents();
};

#define TRESULT_APP_CELEBS 3

class TResultApp
{
public:
	CLocation location;
	VCTIME  event;
	DAYDATA details;
	Boolean b_adhika;
	int celeb_gy[TRESULT_APP_CELEBS];
	VCTIME celeb_date[TRESULT_APP_CELEBS];
};

class TResultMasaList  
{
public:
	int n_allocated;
	Boolean alloc(int nCountYears);
	class TResultMasa
	{
	public:
		int masa;
		int year;
		VCTIME vc_start;
		VCTIME vc_end;
	};
	TResultMasa * arr;
	VCTIME vc_end;
	VCTIME vc_start;
	int n_countYears;
	int n_countMasa;
	int n_startYear;
	CLocation m_location;
	TResultMasaList();
	virtual ~TResultMasaList();

};


#define CCE_SUN  0x0001
#define CCE_TIT  0x0002
#define CCE_NAK  0x0004
#define CCE_SNK  0x0008
#define CCE_CNJ  0x0010
#define CCE_SORT 0x0100
#define CCE_ALL  0x0fff

#define CCTYPE_DATE 1
#define CCTYPE_S_ARUN 10
#define CCTYPE_S_RISE 11
#define CCTYPE_S_NOON 12
#define CCTYPE_S_SET  13
#define CCTYPE_TITHI  20
#define CCTYPE_NAKS   21
#define CCTYPE_SANK   22
#define CCTYPE_CONJ   23

void CalcEvents(TResultEvents &inEvents, CLocation &loc, VCTIME va, VCTIME ve, UInt32 fOptions);
void FormatTodayInfo(VCTIME vc, CLocation & loc, TString &str);
void FormatAppDayXML(TResultApp &, TString &strResult);
void CalcAppDay(CLocation &, VCTIME, TResultApp &);
int CalcCalendar(TResultCalendar &daybuff, CLocation & loc, VCTIME date, int nDaysCount);
int FormatCalendarOld(TResultCalendar &, TString &);
int FormatCalendarICAL(TResultCalendar &calendar, TString &str);
int FormatCalendarVCAL(TResultCalendar &calendar, TString &str);
int FormatCalendarCSV(TResultCalendar &daybuff, TString &m_text);
int FormatEventsTextXML(TResultEvents &, TString &, TString &);

int ProgressWindowCreate();
int ProgressWindowSetRange(int nMin, int nMax);
int ProgressWindowSetPos(int nPos);
int ProgressWindowClose();

const char* WriteCalendarXml(TResultCalendar &, FILE * f);
const char* WriteXML_FirstDay_Year(FILE *, CLocation &loc, VCTIME vc);
const char* WriteXML_Sankrantis(FILE *, CLocation &loc, VCTIME, VCTIME);
const char* WriteXML_Naksatra(FILE *, CLocation &loc, VCTIME, int);
const char* WriteXML_Tithi(FILE *, CLocation &loc, VCTIME);
const char* WriteXML_MasaStart(FILE *, CLocation &loc, VCTIME, int nPaksa);
const char* WriteXML_GaurabdaTithi(FILE * fout, CLocation &loc, VATIME vaStart, VATIME vaEnd);
const char* WriteXML_GaurabdaNextTithi(FILE * fout, CLocation &loc, VCTIME vcStart, VATIME vaStart);
VCTIME GetFirstDayOfCurrentMasa(EARTHDATA earth, VCTIME vc, int &nMasa, int &nGyear);

int is_daylight_time(VCTIME vc, int pndex);
int GetSankrantiType();

void FormatInput(VCTIME vc);
void FormatInput(TString &s);	
void FormatInput(DAYTIME dt);
void FormatInput(int n);
void FormatInput(double d);	



#endif
