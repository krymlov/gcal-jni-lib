// CommandLineVCal.cpp: implementation of the CCommandLineVCal class.
// PORTABLE
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <ctype.h>

#include "platform.h"
#include "apc.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wconversion-null"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int gnLastError = 0;


//////////////////////////////////////////////////////////////////////
//

int ApcString_SetArgLastError(int i)
{
	return (gnLastError = i);
}

//////////////////////////////////////////////////////////////////////
//

int ApcString_GetArg_Year(const char * str, int &nYear)
{
	int n = atoi(str);

	if (n < 1600)
	{
		return ApcString_SetArgLastError(10);
	}

	if (n > 3999)
		return ApcString_SetArgLastError(11);

	nYear = n;

	return NULL;

}

//////////////////////////////////////////////////////////////////////
//

int ApcString_GetArg_Int(const char * str, int &nInteger)
{
	nInteger = atoi(str);

	return NULL;
}

//////////////////////////////////////////////////////////////////////
//

int AvcComboMasaToMasa(int);

int ApcString_GetArg_Masa(const char * str)
{
	int nMasa = 0;

	const char * masaname[14] = 
	{
		"visnu",
		"madhusudana",
		"trivikrama",
		"vamana",
		"sridhara",
		"hrsikesa",
		"padmanabha",
		"damodara",
		"kesava",
		"narayana",
		"madhava",
		"govinda",
		"purusottama"
	};


	for(int i = 0; i < 13; i++)
	{
		if (stricmp(masaname[i], str) == 0)
		{
			return AvcComboMasaToMasa(i);
		}
	}

	nMasa = atoi(str);

	if (nMasa != 0)
	{
		return AvcComboMasaToMasa(nMasa - 1);
	}

	return 11;
}

int ApcString_GetArg_EarthPos(const char * str, double &Latitude, double &Longitude)
{
	double l = 0.0;
	double sig = 1.0;
	double coma = 10.0;
	bool after_coma = false;
	bool is_deg = false;
	bool bNorth = false;
	bool bLat = false;
	bool bLon = false;

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
			bNorth = true;
			break;
		case 's': case 'S':
			bNorth = true;
			after_coma = true;
			is_deg = true;
			sig = -1.0;
			break;
		case 'e': case 'E':
			bNorth = false;
			after_coma = true;
			is_deg = true;
			sig = 1.0;
			break;
		case 'w': case 'W':
			bNorth = false;
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
		case ';':
			if (bNorth == true)
			{
				bLat = true;
				Latitude = l * sig;
				bNorth = false;
			}
			else
			{
				bLon = true;
				Longitude = l * sig;
				bNorth = true;
			}
			l = 0.0;
			sig = 1.0;
			after_coma = false;
			is_deg = false;
			coma = 10.0;
			break;
		default:
			break;
		}
		s++;
	}

	if (bNorth == true)
	{
		bLat = true;
		Latitude = l * sig;
	}
	else
	{
		bLon = true;
		Longitude = l * sig;
	}

	return NULL;
}

int ApcString_GetArg_TimeZone(const char * str, double &TimeZone)
{
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
		case 'W':
			after_coma = true;
			sig = -1.0;
			is_deg = true;
			break;
		case 'E':
			after_coma = true;
			sig = 1.0;
			is_deg = true;
			break;
		case '-':
			//after_coma = true;
			sig = -1.0;
			break;
		case '+':
			//after_coma = true;
			sig = 1.0;
			break;
		case '.':
			after_coma = true;
			is_deg = false;
			break;
		case ':':
			after_coma = true;
			is_deg = true;
			break;
		default:
			return ApcString_SetArgLastError(14);
		}
		s++;
	}

	TimeZone = l * sig;

	return NULL;
}

int ApcString_GetArg_Tithi(const char * str)
{
	const char * tithi[] = {
		"Pratipat",
		"Dvitiya",
		"Tritiya",
		"Caturthi",
		"Pancami",
		"Sasti",
		"Saptami",
		"Astami",
		"Navami",
		"Dasami",
		"Ekadasi",
		"Dvadasi",
		"Trayodasi",
		"Caturdasi",
		"Amavasya",
		"Pratipat",
		"Dvitiya",
		"Tritiya",
		"Caturthi",
		"Pancami",
		"Sasti",
		"Saptami",
		"Astami",
		"Navami",
		"Dasami",
		"Ekadasi",
		"Dvadasi",
		"Trayodasi",
		"Caturdasi",
		"Purnima"
	};

	int i;

	for(i = 0; i < 30; i++)
	{
		if (stricmp(tithi[i], str) == 0)
		{
			return i % 15;
		}
	}

	i = atoi(str);
	if (i > 0)
	{
		return i - 1;
	}

	return 0;
}

int ApcString_GetArg_Paksa(const char * str)
{
	if ((str[0] == char('g')) || (str[0] == char('G')) || (str[0] == char('1')))
	{
		return 1;
	}

	return 0;
}

int ApcString_GetArg_Date(const char * str, VCTIME &vc)
{
	char szt[32];
	TString s[4];
	int n, p, len;
	int c = 0;

	p = strlen(str);
	for(n = p - 1; n >= 0; n--)
	{
		if (str[n] == '-' || str[n]=='/' || n == 0)
		{
			if (n == 0)
				n--;
			len = p - n - 1;
			if (len > 30)
				len = 30;
			strncpy(szt, str + n + 1, len);
			szt[len] = 0;
			//TRACE2("###arg %d = %s\n", c, szt);
			s[c] = szt;
			c++;
			p = n;
		}
	}

	vc.day = atoi(s[2].c_str());
	if (vc.day == 0)
		vc.day = 1;
	vc.month = atoi(s[1].c_str());
	if (vc.month == 0)
		vc.month = 1;
	vc.year = atoi(s[0].c_str());

	return NULL;
}

int ApcString_GetArg_VaisnDate(const char * str, VATIME &va)
{
	char szt[32];
	TString s[4];
	int n, p, len;
	int c = 0;

	p = strlen(str);
	for(n = p - 1; n >= 0; n--)
	{
		if (str[n] == '-' || str[n] == '/' || n == 0)
		{
			if (n == 0)
				n--;
			len = p - n - 1;
			if (len > 30)
				len = 30;
			strncpy(szt, str + n + 1, len);
			szt[len] = 0;
			//TRACE2("###arg %d = %s\n", c, szt);
			s[c] = szt;
			c++;
			p = n;
		}
	}

	va.tithi = ApcString_GetArg_Tithi(s[3].c_str()) + ApcString_GetArg_Paksa(s[2].c_str())*15;
	va.masa = ApcString_GetArg_Masa(s[1].c_str());
	va.gyear = atoi(s[0].c_str());

	return NULL;
}


int ApcString_GetArg_Interval(const char * pszText, int &A, int &B)
{
	int * p = &A;

	A = 0;
	B = 0;

	const char * t = pszText;

	while(t && *t)
	{
		if ((*t == char('-')) || (*t == char(':')))
		{
			p = &B;
		}
		else if (isdigit(char(*t)))
		{
			*p = (*p) * 10 + (*t - char('0'));
		}
		else
			return 1;
		t++;
	}

	if (A == 0)
		A = B;
	else if (B == 0)
		B = A;
	
	return 0;
}


int ApcString_GetArg_Time(const char * str, VCTIME &vc)
{
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
		case ':':
			after_coma = true;
			is_deg = true;
			break;
		default:
			return ApcString_SetArgLastError(14);
		}
		s++;
	}

	vc.shour = l / 24.0;

	return NULL;
}
