
#include "astro.h"
#include "avc.h"
#include "ttimezone.h"
#include "stdlib.h"

// PORTABLE
char XMLOut[64 * 1024];
int AvcMasaToComboMasa(int nMasa);

const char* WriteXML_FirstDay_Year(FILE * fout, CLocation & loc, VCTIME vcStart)
{
	vcStart = GetFirstDayOfYear((EARTHDATA)loc, vcStart.year);
	vcStart.InitWeekDay();

	// write
	strcpy(XMLOut,"<xml>\n"); 
	strcat(XMLOut,"\t<request name=\"FirstDay\" version=\"");
	FormatInput(gstr[130]);
	strcat(XMLOut,"\">\n");
	strcat(XMLOut,"\t\t<arg name=\"longitude\" val=\"");
	FormatInput(loc.m_fLongitude);
	strcat(XMLOut, "\" />\n");	
	strcat(XMLOut,"\t\t<arg name=\"latitude\" val=\"");
	FormatInput(loc.m_fLatitude);
	strcat(XMLOut, "\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"year\" val=\"");	
	FormatInput(vcStart.year);
	strcat(XMLOut, "\" />\n");
	strcat(XMLOut, "\t</request>\n");
	strcat(XMLOut,"\t<result name=\"FirstDay_of_GaurabdaYear\">\n");
	strcat(XMLOut,"\t\t<firstday date=\"");
	FormatInput(vcStart);
	strcat(XMLOut,"\" dayweekid = \"");
	FormatInput(vcStart.dayOfWeek);
	strcat(XMLOut,"\" dayweek=\"");	
	FormatInput(gstr[vcStart.dayOfWeek]);
	strcat(XMLOut, "\" />\n");
	strcat(XMLOut,"\t</result>\n");
	strcat(XMLOut,"</xml>\n");
	
	return XMLOut;
}


const char* WriteXML_Sankrantis(FILE * fout, CLocation & loc, VCTIME vcStart, VCTIME vcEnd)
{
	VCTIME d;
	DAYTIME dt;
	int zodiac;

	d = vcStart;

	strcpy(XMLOut,"<xml>\n"); 
	strcat(XMLOut,"\t<request name=\"Sankranti\" version=\""); 
	FormatInput(gstr[130]);
	strcat(XMLOut,"\">\n");
	strcat(XMLOut,"\t\t<arg name=\"longitude\" val=\""); 
	FormatInput(loc.m_fLongitude);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"latitude\" val=\""); 
	FormatInput(loc.m_fLatitude);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"timezone\" val=\""); 
	FormatInput(loc.m_fTimezone);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"location\" val=\""); 
	FormatInput(loc.m_strCity);
	strcat(XMLOut," "); 
	FormatInput(loc.m_strCountry);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"startdate\" val=\"");
	FormatInput(vcStart);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"enddate\" val=\"");
	FormatInput(vcEnd);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t</request>\n");
	strcat(XMLOut,"\t<result name=\"SankrantiList\">\n");

	while(d.IsBeforeThis(vcEnd))
	{
		d = GetNextSankranti(d, zodiac);
		d.InitWeekDay();
		strcat(XMLOut,"\t\t<sank date=\"");
		FormatInput(d); 
		strcat(XMLOut,"\" ");
		strcat(XMLOut,"dayweekid=\"");
		FormatInput(d.dayOfWeek );
		strcat(XMLOut,"\" dayweek=\""); 
		FormatInput(gstr[d.dayOfWeek]); 
		strcat(XMLOut,"\" ");

		dt.SetDegTime( 360 * d.shour );

		strcat(XMLOut," time=\"");
		FormatInput(dt); 
		strcat(XMLOut,"\" >\n");
		strcat(XMLOut,"\t\t<zodiac sans=\"");
		strcat(XMLOut,GetSankrantiName(zodiac));
		strcat(XMLOut,"\" eng=\"");
		strcat(XMLOut,GetSankrantiNameEn(zodiac)); 
		strcat(XMLOut, "\" id=\""); 
		FormatInput(zodiac);
		strcat(XMLOut, "\" />\n");
		strcat(XMLOut,"\t\t</sank>\n\n");

		d.NextDay();
		d.NextDay();
	}

	strcat(XMLOut,"\t</result>\n");
	strcat(XMLOut,"</xml>");
	
	return XMLOut;
}

const char* WriteCalendarXml(TResultCalendar &daybuff, FILE * fout)
{
	int k;
	TString str, st;
	VCTIME date;	
	
	VAISNAVADAY * pvd;
	int nPrevMasa = -1;
	int nPrevPaksa = -1;
	
	FILE *stream1;

	strcpy(XMLOut,"<xml>\n");
	strcat(XMLOut,"\t<request name=\"Calendar\" version=\"");
	FormatInput(gstr[130]);
	strcat(XMLOut,"\">\n");
	strcat(XMLOut,"\t\t<arg name=\"longitude\" val=\"");
	FormatInput(daybuff.m_Location.m_fLongitude);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"latitude\" val=\"");
	FormatInput(daybuff.m_Location.m_fLatitude);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"timezone\" val=\"");
	FormatInput(daybuff.m_Location.m_fTimezone);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"startdate\" val=\"");	
	FormatInput(daybuff.m_vcStart);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"daycount\" val=\"");
	FormatInput(daybuff.m_vcCount);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"dst\" val=\"x");
	FormatInput(daybuff.m_Location.m_nDST);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t</request>\n");
	strcat(XMLOut,"\t<result name=\"Calendar\">\n");
	/*if (daybuff.m_Location.m_nDST > 0)
	{
		strcat(XMLOut,"\t<dstsystem name=\"");
		strcat(XMLOut,TTimeZone::GetTimeZoneName(daybuff.m_Location.m_nDST));
		strcat(XMLOut,"\" />\n");
	}*/

	for (k = 0; k < daybuff.m_vcCount; k++)
	{
		pvd = daybuff.GetDay(k);
		if (pvd)
		{
			if (nPrevMasa != pvd->astrodata.nMasa)
			{
				if (nPrevMasa != -1)
				{
					strcat(XMLOut,"\t</masa>\n");
				}
				strcat(XMLOut,"\t<masa name=\"");
				strcat(XMLOut,GetMasaName(pvd->astrodata.nMasa));
				strcat(XMLOut," Masa");
				if (nPrevMasa == ADHIKA_MASA)
				{
					strcat(XMLOut," ");
					FormatInput(gstr[109]);
				}
				strcat(XMLOut,"\"");
				strcat(XMLOut," gyear=\"Gaurabda ");
				FormatInput(pvd->astrodata.nGaurabdaYear); 
				strcat(XMLOut,"\"");
				strcat(XMLOut,">\n");
			}

			nPrevMasa = pvd->astrodata.nMasa;

			// date data
			strcat(XMLOut,"\t<day date=\"");
			FormatInput(pvd->date);
			strcat(XMLOut,"\" dayweekid=\"");
			FormatInput(pvd->date.dayOfWeek);
			strcat(XMLOut,"\" dayweek=\"");
			gstr[pvd->date.dayOfWeek].Left(2, st);			
			strcat(XMLOut,st.c_str());
			strcat(XMLOut,"\">\n");

			// sunrise data
			strcat(XMLOut,"\t\t<sunrise time=\"");
			FormatInput(pvd->astrodata.sun.rise);
			strcat(XMLOut,"\">\n");

			strcat(XMLOut,"\t\t\t<tithi name=\"");
			strcat(XMLOut,GetTithiName(pvd->astrodata.nTithi));	
		
			
			if ((pvd->astrodata.nTithi == 10) || (pvd->astrodata.nTithi == 25) 
				|| (pvd->astrodata.nTithi == 11) || (pvd->astrodata.nTithi == 26))
			{
				if (pvd->ekadasi_parana == false)
				{
					if (pvd->nMhdType == EV_NULL)
					{
						strcat(XMLOut," ");
						FormatInput(gstr[58]);
					}
					else
					{
						strcat(XMLOut," ");
						FormatInput(gstr[59]);
					}
				}
			}

			if ( pvd->astrodata.nTithi < 0 ) {
				str.Format("\" elapse=\"%.1f\" index=\"%d\"/>\n" ,0, 0 );
			} else {
				str.Format("\" elapse=\"%.1f\" index=\"%d\"/>\n",
					pvd->astrodata.nTithiElapse, pvd->astrodata.nTithi % 30 + 1 );
			}

			FormatInput(str);

			str.Format("\t\t\t<naksatra name=\"%s\" elapse=\"%.1f\" />\n"
				,GetNaksatraName(pvd->astrodata.nNaksatra), pvd->astrodata.nNaksatraElapse );
			FormatInput(str);

			str.Format("\t\t\t<yoga name=\"%s\" />\n", GetYogaName(pvd->astrodata.nYoga) );
			FormatInput(str);

			str.Format("\t\t\t<paksa id=\"%c\" name=\"%s\"/>\n", GetPaksaChar(pvd->astrodata.nPaksa), GetPaksaName(pvd->astrodata.nPaksa) );
			FormatInput(str);

			strcat(XMLOut,"\t\t</sunrise>\n");

			strcat(XMLOut,"\t\t<dst offset=\"");
			FormatInput(pvd->nDST);
			strcat(XMLOut,"\" />\n");
			// arunodaya data
			strcat(XMLOut,"\t\t<arunodaya time=\"");
			FormatInput(pvd->astrodata.sun.arunodaya );
			strcat(XMLOut,"\">\n");
			strcat(XMLOut,"\t\t\t<tithi name=\"" );
			strcat(XMLOut,GetTithiName(pvd->astrodata.nTithiArunodaya));
			strcat(XMLOut,"\" />\n");
			strcat(XMLOut,"\t\t</arunodaya>\n");

			str.Empty();

			strcat(XMLOut,"\t\t<noon time=\"");
			FormatInput(pvd->astrodata.sun.noon);
			strcat(XMLOut,"\" />\n");

			strcat(XMLOut,"\t\t<sunset time=\"");
			FormatInput(pvd->astrodata.sun.set);
			strcat(XMLOut,"\" />\n");

			// moon data
			strcat(XMLOut,"\t\t<moon rise=\"");
			FormatInput(pvd->moonrise);
			strcat(XMLOut,"\" set=\"");
			FormatInput(pvd->moonset);
			strcat(XMLOut,"\" />\n");

			if (pvd->ekadasi_parana)
			{
				double h1, m1, h2, m2;
				m1 = modf(pvd->eparana_time1, &h1);
				if (pvd->eparana_time2 >= 0.0)
				{
					m2 = modf(pvd->eparana_time2, &h2);
					str.Format("\t\t<parana from=\"%02d:%02d\" to=\"%02d:%02d\" />\n", int(h1), int(m1*60), int(h2), int(m2*60));
				}
				else
				{
					str.Format("\t\t<parana after=\"%02d:%02d\" />\n", int(h1), int(m1*60) );
				}
				strcat(XMLOut,str);
			}
			str.Empty();

			if (pvd->festivals.GetLength() > 0)
			{
				int i = pvd->GetHeadFestival();
				int nFestClass;
				TString str2;
				while(pvd->GetNextFestival(i, str2))
				{
					if (str2.GetLength() > 1)
					{
						nFestClass = pvd->GetFestivalClass(str2);
						str.Format("\t\t<festival name=\"%s\" class=\"%d\"/>\n", str2.c_str(), nFestClass);
						strcat(XMLOut,str);
					}
				}
			}

			if (pvd->nFastType != FAST_NULL)
			{
				switch (pvd->nFastType)
				{
				case FAST_EKADASI:
					//xml << "\t\t<fast type=\"Fasting for ";
					//xml << pvd->ekadasi_vrata_name;
					//xml << "\" mark=\"*\"/>\n";
					//break;
				case FAST_NOON:
					//xml << "\t\t<fast type=\"(Fast till noon)\"  mark=\"*\"/>\n";
					//xml << "\t\t<fast type=\"\" mark=\"*\" />\n";
					//break;
				case FAST_SUNSET:
					//xml << "\t\t<fast type=\"(Fast till sunset)\" mark=\"*\" />\n";
					//xml << "\t\t<fast type=\"\" mark=\"*\" />\n";
					//break;
				case FAST_MOONRISE:
					//xml << "\t\t<fast type=\"(Fast till moonrise)\" mark=\"*\" />\n";
					//xml << "\t\t<fast type=\"\" mark=\"*\" />\n";
					//break;
				case FAST_DUSK:
					//xml << "\t\t<fast type=\"(Fast till dusk)\" mark=\"*\" />\n";
					//xml << "\t\t<fast type=\"\" mark=\"*\" />\n";
					//break;
				case FAST_MIDNIGHT:
					//xml << "\t\t<fast type=\"(Fast till midnight)\" mark=\"*\" />\n";
					strcat(XMLOut,"\t\t<fast type=\"\" mark=\"*\" />\n");
					break;
				default:
					strcat(XMLOut,"\t\t<fast type=\"\" mark=\"\" />\n");
					break;
				}
			}

			if (pvd->sankranti_zodiac >= 0)
			{
				//double h1, m1, s1;
				//m1 = modf(pvd->sankranti_day.shour*24, &h1);
//				s1 = modf(m1*60, &m1);
				str.Format("\t\t<sankranti rasi=\"%s\" time=\"%02d:%02d:%02d\" />\n"
					, GetSankrantiName(pvd->sankranti_zodiac), pvd->sankranti_day.GetHour()
					, pvd->sankranti_day.GetMinute(), pvd->sankranti_day.GetSecond());
				strcat(XMLOut,str);
			}

			if (pvd->was_ksaya)
			{
				double h1, m1, h2, m2;
				m1 = modf(pvd->ksaya_time1*24, &h1);
				m2 = modf(pvd->ksaya_time2*24, &h2);
				str.Format("\t\t<ksaya from=\"%02d:%02d\" to=\"%02d:%02d\" />\n", int(h1), abs(int(m1*60)), int(h2), abs(int(m2*60)));
				strcat(XMLOut,str);
			}

			if (pvd->is_vriddhi)
			{
				strcat(XMLOut,"\t\t<vriddhi sd=\"yes\" />\n");
			}
			else
			{
				strcat(XMLOut, "\t\t<vriddhi sd=\"no\" />\n");
			}

			if (pvd->nCaturmasya & CMASYA_PURN_MASK)
			{
				strcat(XMLOut, "\t\t<caturmasya day=\""); 
					strcat(XMLOut,(((pvd->nCaturmasya & CMASYA_PURN_MASK_DAY)) > 1 ? "last" : "first"));
					strcat(XMLOut, "\" month=\"" );
					FormatInput(int((pvd->nCaturmasya & CMASYA_PURN_MASK_MASA) >> 4)); 
					strcat(XMLOut,  "\" system=\"PURNIMA\" />\n");
			}

			if (pvd->nCaturmasya & CMASYA_PRAT_MASK)
			{
					strcat(XMLOut,"\t\t<caturmasya day=\""); 
					strcat(XMLOut,(((pvd->nCaturmasya & CMASYA_PRAT_MASK_DAY) >> 8) > 1 ? "last" : "first"));
					strcat(XMLOut,"\" month=\""); 
					FormatInput(int((pvd->nCaturmasya & CMASYA_PRAT_MASK_MASA) >> 12));
					strcat(XMLOut, "\" system=\"PRATIPAT\" />\n");
			}

			if (pvd->nCaturmasya & CMASYA_EKAD_MASK)
			{
				str.Format("\t<caturmasya day=\"%s\" month=\"%d\" system=\"EKADASI\" />\n"
					, ((pvd->nCaturmasya & CMASYA_EKAD_MASK_DAY) >> 16) > 1 ? "last" : "first"
					, int((pvd->nCaturmasya & CMASYA_EKAD_MASK_MASA) >> 20));
				strcat(XMLOut, str);
			}

			strcat(XMLOut, "\t</day>\n\n");
		}

		date.shour = 0;
		date.NextDay();
	}

	strcat(XMLOut, "\t</masa>\n");
	strcat(XMLOut, "</result>\n");
	strcat(XMLOut,"</xml>\n");	
	
	return XMLOut;
}	

const char* WriteXML_Naksatra(FILE * fout, CLocation &loc, VCTIME vc, int nDaysCount)
{
	TString str;
	VCTIME date;

	strcpy(XMLOut, "<xml>\n");
	strcat(XMLOut, "\t<request name=\"Naksatra\" version=\"");
	FormatInput(gstr[130]);
	strcat(XMLOut,"\">\n");
	strcat(XMLOut,"\t\t<arg name=\"longitude\" val=\"");
	FormatInput(loc.m_fLongitude);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"latitude\" val=\"");
	FormatInput(loc.m_fLatitude);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"timezone\" val=\"");
	FormatInput(loc.m_fTimezone);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"startdate\" val=\"");
	FormatInput(vc);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"daycount\" val=\"");
	FormatInput(nDaysCount);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t</request>\n");
	strcat(XMLOut,"\t<result name=\"Naksatra\">\n");

	VCTIME d = vc;
	d.tzone = loc.m_fTimezone;
	VCTIME dn;
	DAYTIME dt;
	SUNDATA sun;
	int nak;
	EARTHDATA earth = (EARTHDATA)loc;

	for(int i = 0; i < 30; i++)
	{
		nak = GetNextNaksatra(earth, d, dn);
		d = dn;
		strcat(XMLOut,"\t\t<day date=\"");
		FormatInput(d);
		strcat(XMLOut,"\">\n");
		
		strcat(XMLOut,"\t\t\t<naksatra id=\"");
		FormatInput(nak);
		strcat(XMLOut,"\" name=\"");
		strcat(XMLOut,GetNaksatraName(nak));
		strcat(XMLOut,"\"\n");
		dt.SetDegTime( d.shour * 360);
		
		strcat(XMLOut,"\t\t\t\tstarttime=\"");
		FormatInput(dt);
		strcat(XMLOut,"\" />\n");
		
		// sunrise time get
		SunCalc(d, earth, sun);
		//time_print(str, sun.rise);
		//m_list.SetItemText(n, 3, str);
		strcat(XMLOut, "\t\t\t<sunrise time=\"");
		FormatInput(sun.rise);
		strcat(XMLOut,"\" />\n");

		strcat(XMLOut,"\t\t</day>\n");
		// increment for non-duplication of naksatra
		d = dn;
		d.shour += 1.0/8.0;
	}


	strcat(XMLOut,"\t</result>\n");
	strcat(XMLOut,"</xml>\n");
	
	return XMLOut;
}

const char* WriteXML_Tithi(FILE * fout, CLocation &loc, VCTIME vc)
{
	TString str;
	VCTIME date;

	strcpy(XMLOut,"<xml>\n");
	strcat(XMLOut, "\t<request name=\"Tithi\" version=\"");
	FormatInput(gstr[130]);
	strcat(XMLOut,"\">\n");
	strcat(XMLOut,"\t\t<arg name=\"longitude\" val=\"");
	FormatInput(loc.m_fLongitude);
	strcat(XMLOut, "\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"latitude\" val=\"");
	FormatInput(loc.m_fLatitude);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"timezone\" val=\"");
	FormatInput(loc.m_fTimezone);
	strcat(XMLOut, "\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"startdate\" val=\"");
	FormatInput(vc);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t</request>\n");
	strcat(XMLOut,"\t<result name=\"Tithi\">\n");

	VCTIME d = vc;
	VCTIME d1, d2;
	d.tzone = loc.m_fTimezone;
	VCTIME dn;
	DAYTIME dt;
	EARTHDATA earth = (EARTHDATA)loc;

	DAYDATA day;

	DayCalc(vc, earth, day);

	d.shour = day.sun.sunrise_deg/360.0 + loc.m_fTimezone/24.0;

	GetPrevTithiStart(earth, d, d1);
	GetNextTithiStart(earth, d, d2);

	{
		dt.SetDegTime( d1.shour * 360 );
		// start tithi at t[0]
		strcat(XMLOut,"\t\t<tithi\n\t\t\tid=\"");
		FormatInput(day.nTithi);
		strcat(XMLOut,"\"\n");
		strcat(XMLOut,"\t\t\tname=\"");
		strcat(XMLOut,GetTithiName(day.nTithi)); 
		strcat(XMLOut,"\"\n");
		strcat(XMLOut,"\t\t\tstartdate=\"");
		FormatInput(d1);
		strcat(XMLOut,"\"\n");
		strcat(XMLOut,"\t\t\tstarttime=\"");
		FormatInput(dt);
		strcat(XMLOut,"\"\n");
	
		dt.SetDegTime( d2.shour * 360 );
		strcat(XMLOut,"\t\t\tenddate=\"");
		FormatInput(d2);
		strcat(XMLOut,"\"\n");
		strcat(XMLOut,"\t\t\tendtime=\"");
		FormatInput(dt);
		strcat(XMLOut,"\"\n />");
	}

	strcat(XMLOut,"\t</result>\n");
	strcat(XMLOut,"</xml>\n");
		
	return XMLOut;
}

const char* WriteXML_MasaStart(FILE * fout, CLocation &loc, VCTIME vc, int nPaksa)
{
	TString str;
	VCTIME date;
	VCTIME d = vc;
	VCTIME d1, d2;
	d.tzone = loc.m_fTimezone;
	VCTIME dn;
	DAYTIME dt;
	EARTHDATA earth = (EARTHDATA)loc;
	VCTIME vcOrig = vc;
	VATIME va;

	VCTIMEtoVATIME(vc, va, earth);
	if (nPaksa == 0)
	{
		if (va.masa == 12)
		{
			va.tithi = 15;
		}
		else
		{
			va.tithi = 0;
		}
	}
	else
	{
		if (va.tithi >= 15) va.tithi = 15;
		else va.tithi = 0;
	}

	VCTIME vcStart;
	VATIMEtoVCTIME(va, vcStart, earth);
	vcStart.InitWeekDay();
	char szLoc[64];
	sprintf(szLoc, "%f %f", earth.longitude_deg, earth.latitude_deg);

	sprintf(XMLOut, "<xml>\n\t<request name=\"FirstMasaDay\" version=\"%s\">\n" 
		"\t\t<arg name=\"longitude\" val=\"%f\" />\n"
		"\t\t<arg name=\"latitude\" val=\"%f\" />\n"
		"\t\t<arg name=\"year\" val=\"%d\" />\n"
		"\t\t<arg name=\"month\" val=\"%d\" />\n"
		"\t\t<arg name=\"day\" val=\"%d\" />\n"
		"\t</request>\n"
		"\t<result name=\"FirstDay_of_Masa\">\n"
		"\t\t<masabegin date=\"%d %s %d\" dayweekid = \"%d\" dayweek=\"%s\" />\n"
		"\t\t<masastart>\n"
		"\t\t\t<year>%d</year>\n"
		"\t\t\t<month>%d</month>\n"
		"\t\t\t<day>%d</day>\n"
		"\t\t\t<weekday>%d</weekday>\n"
		"\t\t\t<masa>%d</masa>\n"
		"\t\t\t<masaname>%s</masaname>\n"
		"\t\t\t<gyear>%d</gyear>\n"
		"\t\t</masastart>\n"
		"\t</result>\n"
		"</xml>\n",
		gstr[130].c_str(), earth.longitude_deg, earth.latitude_deg, (int)vcOrig.year, (int)vcOrig.month,
		vcOrig.day, vcStart.day, AvcGetMonthAbr(vcStart.month), vcStart.year ,
		vcStart.dayOfWeek, gstr[vcStart.dayOfWeek].c_str(), vcStart.year, vcStart.month,
		vcStart.day, vcStart.dayOfWeek, AvcMasaToComboMasa(va.masa), GetMasaName(va.masa),
		va.gyear);

	return XMLOut;
}

const char* WriteXML_GaurabdaTithi(FILE * fout, CLocation &loc, VATIME vaStart, VATIME vaEnd)
{
	int gyearA = vaStart.gyear;
	int gyearB = vaEnd.gyear;
	int gmasa = vaStart.masa;
	int gpaksa = vaStart.tithi / 15;
	int gtithi = vaStart.tithi % 15;
	
	if (gyearB < gyearA)
		gyearB = gyearA;


	strcpy(XMLOut,"<xml>\n");
	strcat(XMLOut,"\t<request name=\"Tithi\" version=\"");
	FormatInput(gstr[130]);
	strcat(XMLOut,"\">\n");
	strcat(XMLOut,"\t\t<arg name=\"longitude\" val=\"");
	FormatInput(loc.m_fLongitude);
	strcat(XMLOut, "\" />\n");
	strcat(XMLOut, "\t\t<arg name=\"latitude\" val=\"" );
	FormatInput(loc.m_fLatitude);
	strcat(XMLOut, "\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"timezone\" val=\"");
	FormatInput(loc.m_fTimezone);
	strcat(XMLOut, "\" />\n");
	if (gyearA > 1500)
	{
		strcat(XMLOut,"\t\t<arg name=\"year-start\" val=\"");
		FormatInput(gyearA);
		strcat(XMLOut,"\" />\n");
		strcat(XMLOut, "\t\t<arg name=\"year-end\" val=\"");
		FormatInput((gyearB));
		strcat(XMLOut,"\" />\n");
	}
	else
	{
		strcat(XMLOut,"\t\t<arg name=\"gaurabdayear-start\" val=\"");
		FormatInput(gyearA );
		strcat(XMLOut,"\" />\n");
		strcat(XMLOut,"\t\t<arg name=\"gaurabdayear-end\" val=\"");
		FormatInput(gyearB); 
		strcat(XMLOut, "\" />\n");
	}
	strcat(XMLOut,"\t\t<arg name=\"masa\" val=\"");
	FormatInput(gmasa);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"paksa\" val=\"");
	FormatInput(gpaksa);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"tithi\" val=\"");
	FormatInput(gtithi);
	strcat(XMLOut, "\" />\n");
	strcat(XMLOut,"\t</request>\n");
	strcat(XMLOut,"\t<result name=\"Tithi\">\n");

	EARTHDATA earth = (EARTHDATA)loc;
	VCTIME vcs, vce, today;
	SUNDATA sun;
	int A, B;
	double sunrise;
	DAYDATA day;
	int oTithi, oPaksa, oMasa, oYear;

	if (gyearA > 1500)
	{
		A = gyearA - 1487;
		B = gyearB - 1485;
	}
	else
	{
		A = gyearA;
		B = gyearB;
	}

	for(; A <= B; A++)
	{
		vcs = CalcTithiEnd(A, gmasa, gpaksa, gtithi, earth, vce);
		if (gyearA > 1500)
		{
			if ((vcs.year < gyearA) || (vcs.year > gyearB))
				continue;
		}
		oTithi = gpaksa*15 + gtithi;
		oMasa = gmasa;
		oPaksa = gpaksa;
		oYear = 0;
		strcat(XMLOut,"\t<celebration\n");

		strcat(XMLOut,"\t\trtithi=\"");
		strcat(XMLOut,GetTithiName(oTithi));
		strcat(XMLOut,"\"\n");
		strcat(XMLOut,"\t\trmasa=\"");
		strcat(XMLOut,GetMasaName(oMasa));
		strcat(XMLOut,"\"\n");
		strcat(XMLOut,"\t\trpaksa=\"");
		strcat(XMLOut,(oPaksa ? "Gaura" : "Krsna"));
		strcat(XMLOut,"\"\n");

		today = vcs;
		today.shour = 0.5;
		SunCalc(today, earth, sun);
		sunrise = (sun.sunrise_deg + loc.m_fTimezone*15.0)/360;
		if (sunrise < vcs.shour)
		{
			today = vce;
			SunCalc(today, earth, sun);
			sunrise = (sun.sunrise_deg + loc.m_fTimezone*15.0)/360;
			if (sunrise < vce.shour)
			{
				// normal type
				vcs.NextDay();
				strcat(XMLOut, "\t\ttype=\"normal\"\n");
			}
			else
			{
				// ksaya
				vcs.NextDay();
				DayCalc(vcs, earth, day);
				oTithi = day.nTithi;
				oPaksa = day.nPaksa;
				oMasa = MasaCalc(vcs, day, earth, oYear);
				strcat(XMLOut,"\t\ttype=\"ksaya\"\n");
			}
		}
		else
		{
			// normal, alebo prvy den vriddhi
			today = vce;
			SunCalc(today, earth, sun);
			if ((sun.sunrise_deg + loc.m_fTimezone*15.0)/360 < vce.shour)
			{
				// first day of vriddhi type
				strcat(XMLOut,"\t\ttype=\"vriddhi\"\n");
			}
			else
			{
				// normal
				strcat(XMLOut,"\t\ttype=\"normal\"\n");
			}
		}
		strcat(XMLOut,"\t\tdate=\"");
		FormatInput(vcs);
		strcat(XMLOut,"\"\n");
		strcat(XMLOut,"\t\totithi=\"");
		strcat(XMLOut,GetTithiName(oTithi));
		strcat(XMLOut,"\"\n");
		strcat(XMLOut,"\t\tomasa=\"");
		strcat(XMLOut,GetMasaName(oMasa));
		strcat(XMLOut,"\"\n");
		strcat(XMLOut,"\t\topaksa=\"");
		strcat(XMLOut,GetPaksaName(oPaksa));
		strcat(XMLOut,"\"\n");
		strcat(XMLOut,"\t/>\n");
	}

	strcat(XMLOut,"\t</result>\n");
	strcat(XMLOut, "</xml>\n");
	return XMLOut;
}

const char* WriteXML_GaurabdaNextTithi(FILE * fout, CLocation &loc, VCTIME vcStart, VATIME vaStart)
{
	int gmasa, gpaksa, gtithi;
	
	gmasa = vaStart.masa;
	gpaksa = vaStart.tithi / 15;
	gtithi = vaStart.tithi % 15;

	strcpy(XMLOut,"<xml>\n");
	strcat(XMLOut,"\t<request name=\"Tithi\" version=\"");
	FormatInput(gstr[130]);
	strcat(XMLOut,"\">\n");
	strcat(XMLOut,"\t\t<arg name=\"longitude\" val=\"");
	FormatInput(loc.m_fLongitude);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"latitude\" val=\"");
	FormatInput(loc.m_fLatitude);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"timezone\" val=\"");
	FormatInput(loc.m_fTimezone);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"start date\" val=\"");
	FormatInput(vcStart);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"masa\" val=\"");
	FormatInput(gmasa);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"paksa\" val=\""); 
	FormatInput(gpaksa);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t\t<arg name=\"tithi\" val=\""); 
	FormatInput(gtithi);
	strcat(XMLOut,"\" />\n");
	strcat(XMLOut,"\t</request>\n");
	strcat(XMLOut,"\t<result name=\"Tithi\">\n");

	EARTHDATA earth = (EARTHDATA)loc;
	VCTIME vcs, vce, today;
	SUNDATA sun;
	int A;
	double sunrise;
	DAYDATA day;
	int oTithi, oPaksa, oMasa, oYear;

	today = vcStart;
	today.PreviousDay();
	vcStart -= 15;
	for(A = 0; A <= 3; A++)
	{
		vcs = CalcTithiEndEx(vcStart, 0, gmasa, gpaksa, gtithi, earth, vce);
		if (!vcs.IsBeforeThis(today))
		{
			oTithi = gpaksa*15 + gtithi;
			oMasa = gmasa;
			oPaksa = gpaksa;
			oYear = 0;
			strcat(XMLOut,"\t<celebration\n");
	
			strcat(XMLOut,"\t\trtithi=\"");
			strcat(XMLOut,GetTithiName(oTithi));
			strcat(XMLOut,"\"\n");
			strcat(XMLOut,"\t\trmasa=\"");
			strcat(XMLOut,GetMasaName(oMasa));
			strcat(XMLOut,"\"\n");
			strcat(XMLOut, "\t\trpaksa=\"");
			strcat(XMLOut,GetPaksaName(oPaksa));
			strcat(XMLOut,"\"\n");
			// test ci je ksaya
			today = vcs;
			today.shour = 0.5;
			SunCalc(today, earth, sun);
			sunrise = (sun.sunrise_deg + loc.m_fTimezone*15.0)/360;
			if (sunrise < vcs.shour)
			{
				today = vce;
				SunCalc(today, earth, sun);
				sunrise = (sun.sunrise_deg + loc.m_fTimezone*15.0)/360;
				if (sunrise < vce.shour)
				{
					// normal type
					vcs.NextDay();
					strcat(XMLOut,"\t\ttype=\"normal\"\n");
				}
				else
				{
					// ksaya
					vcs.NextDay();
					DayCalc(vcs, earth, day);
					oTithi = day.nTithi;
					oPaksa = day.nPaksa;
					oMasa = MasaCalc(vcs, day, earth, oYear);
					strcat(XMLOut,"\t\ttype=\"ksaya\"\n");
				}
			}
			else
			{
				// normal, alebo prvy den vriddhi
				today = vce;
				SunCalc(today, earth, sun);
				if ((sun.sunrise_deg + loc.m_fTimezone*15.0)/360 < vce.shour)
				{
					// first day of vriddhi type
					strcat(XMLOut,"\t\ttype=\"vriddhi\"\n");
				}
				else
				{
					// normal
					strcat(XMLOut,"\t\ttype=\"normal\"\n");
				}
			}
			strcat(XMLOut,"\t\tdate=\"");
			FormatInput(vcs);
			strcat(XMLOut,"\"\n");
			strcat(XMLOut,"\t\totithi=\"");
			strcat(XMLOut,GetTithiName(oTithi));
			strcat(XMLOut,"\"\n");
			strcat(XMLOut,"\t\tomasa=\"");
			strcat(XMLOut,GetMasaName(oMasa));
			strcat(XMLOut, "\"\n");
			strcat(XMLOut,"\t\topaksa=\"");
			strcat(XMLOut,GetPaksaName(oPaksa)); 
			strcat(XMLOut, "\"\n");
			strcat(XMLOut,"\t/>\n");
			break;
		}
		else
		{
			vcStart = vcs;
			vcs.NextDay();
		}
	}

	strcat(XMLOut,"\t</result>\n");
	strcat(XMLOut,"</xml>\n");
	
	return XMLOut;
}


void FormatInput(TString &s)
{	
	strcat(XMLOut,s.c_str());	
}

void FormatInput(double d)
{
	TString s;
	s.Format("%+.5f", d);		
	strcat(XMLOut,s.c_str());
}

void FormatInput(int n)
{
	TString s;
	s.Format("%d", n);	
	strcat(XMLOut,s.c_str());
}

void FormatInput(DAYTIME dt)
{
	TString s;
	if (dt.hour >= 0)
		s.Format("%02d:%02d:%02d", dt.hour, dt.min, dt.sec);
	else
		s = "N/A";	
	strcat(XMLOut,s.c_str());
}


void FormatInput(VCTIME vc)
{
	TString s;
	s.Format("%d %s %d", vc.day, AvcGetMonthAbr(vc.month), vc.year);
	strcat(XMLOut,s.c_str());
}
