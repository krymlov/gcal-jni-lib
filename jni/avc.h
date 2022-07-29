#ifndef _AVC_H_GCAL_____
#define _AVC_H_GCAL_____
// PORTABLE
#include "tstring.h"
#include "astro.h"

#define GSTR_APPFOLDER 1
#define GSTR_CONFOLDER 2
#define GSTR_LANFOLDER 3
#define GSTR_TEMFOLDER 4

#define GSTR_CE_FILE   5
#define GSTR_CONF_FILE 6
#define GSTR_LOC_FILE  7
#define GSTR_SSET_FILE 8

class CShowSetting
{
public:
	int val;
	int old_val;
	char * sig;
	char * text;
};


extern TString gstr[];

void InitGlobalStrings(void);

const char * GCalApp_GetFileName(int);


class TLangFileInfo  
{
public:
	TString m_strAcr;
	TString m_strLang;
	TString m_strFile;
	TLangFileInfo * next;
	TLangFileInfo(){ next = NULL; };
	virtual ~TLangFileInfo(){};
	void decode(const char * p)
	{
		m_strAcr.Empty();
		m_strLang.Empty();
		m_strFile.Empty();
		int ndest = 0;
		if (p == NULL) return;
		while(*p)
		{
			if (*p == '#')
				ndest++;
			else
			{
				switch(ndest)
				{
				case 0:
					m_strAcr += *p;
					break;
				case 1:
					m_strLang += *p;
					break;
				default:
					m_strFile += *p;
					break;
				}
			}
		}
	}
	void encode(TString &str)
	{
		str = m_strAcr;
		str += "#";
		str += m_strLang;
		str += "#";
		str += m_strFile;
	}

};

class TLangFileList
{
public:
	TLangFileInfo * list;
	TLangFileInfo * add()
	{
		if (list == NULL)
		{
			list = new TLangFileInfo;
			return list;
		}
		TLangFileInfo * p = list;
		while(p->next)
			p = p->next;
		p->next = new TLangFileInfo;
		return p->next;
	}
	void clear()
	{
		TLangFileInfo * p, * r;
		p = list;
		while(p)
		{
			r = p->next;
			delete p;
			p = r;
		}
		list = NULL;
	}
	TLangFileList() { list = NULL; }
	~TLangFileList() { clear(); }
};


//==============================================================
//
//==============================================================

class CCustomEvent  
{
public:
	CCustomEvent();
	virtual ~CCustomEvent();

	int nTithi;
	int nMasa;
	TString strText;
	CCustomEvent * next;
};

class CCustomEventList
{
public:
	void clear();
	CCustomEvent * list;

	CCustomEventList() { list = NULL; }
	~CCustomEventList() { clear(); };
	CCustomEvent * add(void);
};

//==============================================================
//
//==============================================================

extern CCustomEventList gCustomEventList;

//==============================================================
//
//==============================================================

//extern TString gCustomEventTexts[360];

int CustomEventListReadFile(const char * pszFile);
int CustomEventListWriteFile(const char * pszFile);


class CLocationList
{
	CLocation * list;

public:
	Boolean InitList(const char * pszList);
	Boolean m_bModified;
	void AddTail(CLocation * lc);
	void RemoveAll();
	void RemoveAt(CLocation * rem);
	Boolean SaveAs(const char * lpszFileName, int nType);
	CLocation * GetHeadPosition();
	Boolean Analyze(const char * psz, CLocation &lc);
	Boolean ImportFile(const char * pszFile, Boolean bDeleteCurrent);
	void InitInternal();
	CLocationList();
	virtual ~CLocationList();
};

extern CLocationList theLocs;


class TEventClass
{
public:
	int          group;
	const char * text;
};

class TEventValue
{
public:
	const char * getValueText();
	const char * getClassText();
	int group;
	int value;
	const char * text;
};

#define MAX_CONDS 10

class TEventFinder
{
public:
	TEventValue v[MAX_CONDS];
	TString     vt[MAX_CONDS];
	int         v_num;
	Boolean     b_and;
	TEventFinder()
	{
		v_num = 0;
		b_and = true;
	}
	Boolean Test(VAISNAVADAY * pd);
	Boolean canTest();
	Boolean canAddCondition();
	void removeCondition(int i);
	void addCondition(int,int);
	void addConditionText(const char * szText);
	void Calculate(CLocation &L, int inYear, int inCount);
};

class TFinderBuffer : public TResultCalendar  
{
public:
	int GetUpperBound();
	int GetStartIndex();
	int CalculateFindCalendar(int start_year, int start_month, CLocation &inLoc);
	VAISNAVADAY m_rData[50];
	TFinderBuffer();
	virtual ~TFinderBuffer();

};

const char * GetTithiName(int i);
const char * GetSankrantiName(int i);
const char * GetSankrantiNameEn(int i);
const char * GetNaksatraName(int n);
const char * GetYogaName(int n);
char GetPaksaChar(int i);
const char * GetMasaName(int i);
const char * GetMahadvadasiName(int i);
const char * GetPaksaName(int i);
const char * GetSpecFestivalName(int i);
const char * GetNaksatraChildSylable(int n, int pada);
const char * GetFastingName(int i);
const char * GetRasiChildSylable(int n);
const char * GetEkadasiName(int nMasa, int nPaksa);
Boolean   GetSpecFestivalNameEx(TString &str, int i);
double   AvcGetTimeZone(int nIndex);
int   AvcGetNearestTimeZone(double d);
char * AvcGetTimeZoneName(int n);
const char * AvcGetDSTSignature(int nDST);


void AvcShowSetWriteFile(const char *);
void AvcShowSetReadFile(const char * psz);


#endif
