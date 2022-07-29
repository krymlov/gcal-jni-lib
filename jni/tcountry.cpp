// TCountry.cpp: implementation of the TCountry class.
//
//////////////////////////////////////////////////////////////////////

#include "tcountry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// PORTABLE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const char * TCountry::gcontinents[] = 
{
		"",
		"Europe", //1
		"Asia",   //2
		"Africa", //3
		"America",//4
		"Pacific",//5
		"Indiana",//6
		"Atlantic",//7
		""
};

TCountry TCountry::gcountries[] = 
{
	{"AD", 16708, "Andorra", 1 }, //Andorra
	{"AE", 16709, "United Arab Emirates", 2 }, //Dubai
	{"AF", 16710, "Afghanistan", 2 }, //Kabul
	{"AG", 16711, "Antigua", 4 }, //Antigua
	{"AL", 16716, "Albania", 1 }, //Tirane
	{"AM", 16717, "Armenia", 2 }, //Yerevan
	{"AN", 16718, "Curacao", 4 }, //Curacao
	{"AO", 16719, "Angola", 3 }, //Luanda
	{"AR", 16722, "Argentina", 4 }, //Argentina/Buenos_Aires
	{"AS", 16723, "American Samoa", 5 }, //Pago_Pago
	{"AT", 16724, "Austria", 1 }, //Vienna
	{"AU", 16725, "Australia" }, // Australia/Adelaide
	{"AW", 16727, "Aruba", 4 }, //Aruba
	{"AZ", 16730, "Azerbaijan", 2 }, //Baku
	{"BA", 16961, "Bosnia and Herzegovina", 1 }, //Sarajevo
	{"BB", 16962, "Barbados", 4 }, //Barbados
	{"BD", 16964, "Bangladesh", 2 }, //Dhaka
	{"BE", 16965, "Belgium", 1 }, //Brussels
	{"BF", 16966, "Burkina Faso", 3 }, //Ouagadougou
	{"BG", 16967, "Bulgaria", 1 }, //Sofia
	{"BH", 16968, "Bahrain", 2 }, //Bahrain
	{"BI", 16969, "Burundi", 3 }, //Bujumbura
	{"BJ", 16970, "Benin", 3 }, //Porto-Novo
	{"BL", 16972, "Guadeloupe", 4 }, //Guadeloupe
	{"BN", 16974, "Brunei", 2 }, //Brunei
	{"BO", 16975, "Bolivia", 4 }, //La_Paz
	{"BR", 16978, "Brazil", 4 }, //Belem
	{"BS", 16979, "Bahamas", 4 }, //Nassau
	{"BT", 16980, "Bhutan", 2 }, //Thimphu
	{"BW", 16983, "Botswana", 3 }, //Gaborone
	{"BY", 16985, "Belarus", 1 }, //Minsk
	{"BZ", 16986, "Belize", 4 }, //Belize
	{"CA", 17217, "Canada", 4 }, //Vancouver
	{"CD", 17220, "Democratic Republic of the Congo", 3 }, //Lubumbashi
	{"CF", 17222, "Central African Republic", 3 }, //Bangui
	{"CG", 17223, "Congo", 3 }, //Brazzaville
	{"CH", 17224, "Switzerland", 1 }, //Zurich
	{"CI", 17225, "Ivory Coast", 3 }, //Abidjan
	{"CK", 17227, "Cook Islands", 5 }, //Rarotonga
	{"CL", 17228, "Chile", 4 }, //Santiago
	{"CM", 17229, "Cameroon", 3 }, //Douala
	{"CN", 17230, "China", 2 }, //Urumqi
	{"CO", 17231, "Colombia", 4 }, //Bogota
	{"CR", 17234, "Costa Rica", 4 }, //Costa_Rica
	{"CU", 17237, "Cuba", 4 }, //Havana
	{"CV", 17238, "Cape Verde", 7 }, //Cape_Verde
	{"CY", 17241, "Cyprus", 2 }, //Nicosia
	{"CZ", 17242, "Czech", 1 }, //Prague
	{"DE", 17477, "Germany", 1 }, //Berlin
	{"DJ", 17482, "Djibouty", 3 }, //Djibouti
	{"DK", 17483, "Denmark", 1 }, //Copenhagen
	{"DM", 17485, "Dominica", 4 }, //Dominica
	{"DO", 17487, "Dominican Republic", 4 }, //Santo_Domingo
	{"DZ", 17498, "Algeria", 3 }, //Algiers
	{"EC", 17731, "Ecuador", 4 }, //Guayaquil
	{"EE", 17733, "Estonia", 1 }, //Tallinn
	{"EG", 17735, "Egypt", 3 }, //Cairo
	{"EH", 17736, "Western Sahara", 3 }, //El_Aaiun
	{"ER", 17746, "Eritrea", 3 }, //Asmara
	{"ES", 17747, "Spain", 3 }, //Ceuta
	{"ET", 17748, "Ethiopia", 3 }, //Addis_Ababa
	{"FI", 17993, "Finland", 1 }, //Helsinki
	{"FJ", 17994, "Fiji", 5 }, //Fiji
	{"FM", 17997, "Ponape", 5 }, //Ponape
	{"FO", 17999, "Faroe", 7 }, //Faroe
	{"FR", 18002, "France", 1 }, //Paris
	{"GA", 18241, "Gabon", 3 }, //Libreville
	{"GB", 18242, "United Kingdom", 1 }, //London
	{"GE", 18245, "Georgia", 2 }, //Tbilisi
	{"GF", 18246, "French Guiana", 4 }, //Cayenne
	{"GH", 18248, "Ghana", 3 }, //Accra
	{"GI", 18249, "Gibraltar", 1 }, //Gibraltar
	{"GL", 18252, "Greenland", 4 }, //Godthab
	{"GM", 18253, "Gambia", 3 }, //Banjul
	{"GN", 18254, "Guinea", 3 }, //Conakry
	{"GP", 18256, "Guadeloupe", 4 }, //Guadeloupe
	{"GQ", 18257, "Equatorial Guinea", 3 }, //Malabo
	{"GR", 18258, "Greece", 1 }, //Athens
	{"GT", 18260, "Guatemala", 4 }, //Guatemala
	{"GU", 18261, "Guam", 5 }, //Guam
	{"GW", 18263, "Guinea-Bissau", 3 }, //Bissau
	{"GY", 18265, "Guyana", 4 }, //Guyana
	{"HK", 18507, "Hong Kong", 2 }, //Hong_Kong
	{"HN", 18510, "Honduras", 4 }, //Tegucigalpa
	{"HR", 18514, "Croatia", 1 }, //Zagreb
	{"HT", 18516, "Haiti", 4 }, //Port-au-Prince
	{"HU", 18517, "Hungary", 1 }, //Budapest
	{"ID", 18756, "Indonesia", 2 }, //Makassar
	{"IE", 18757, "Ireland", 1 }, //Dublin
	{"IL", 18764, "Israel", 2 }, //Jerusalem
	{"IN", 18766, "India", 2 }, //Calcutta
	{"IQ", 18769, "Iraq", 2 }, //Baghdad
	{"IR", 18770, "Iran", 2 }, //Tehran
	{"IS", 18771, "Iceland", 7 }, //Reykjavik
	{"IT", 18772, "Italy", 1 }, //Rome
	{"JM", 19021, "Jamaica", 4 }, //Jamaica
	{"JO", 19023, "Jordan", 2 }, //Amman
	{"JP", 19024, "Japan", 2 }, //Tokyo
	{"KE", 19269, "Kenya", 3 }, //Nairobi
	{"KG", 19271, "Kyrgyzstan", 2 }, //Bishkek
	{"KH", 19272, "Cambodia", 2 }, //Phnom_Penh
	{"KI", 19273, "Kiribati", 5 }, //Tarawa
	{"KM", 19277, "Comoro", 6 }, //Comoro
	{"KN", 19278, "Saint Kitts and Nevis", 4 }, //St_Kitts
	{"KP", 19280, "North Korea", 2 }, //Pyongyang
	{"KR", 19282, "South Korea", 2 }, //Seoul
	{"KW", 19287, "Kuwait", 2 }, //Kuwait
	{"KY", 19289, "Cauman", 4 }, //Cayman
	{"KZ", 19290, "Kazakhstan", 2 }, //Oral
	{"LA", 19521, "Laos", 2 }, //Vientiane
	{"LB", 19522, "Lebanon", 2 }, //Beirut
	{"LC", 19523, "Saint Lucia", 4 }, //St_Lucia
	{"LI", 19529, "Liechtenstein", 1 }, //Vaduz
	{"LK", 19531, "Sri Lanka", 2 }, //Colombo
	{"LR", 19538, "Liberia", 3 }, //Monrovia
	{"LS", 19539, "Lesotho", 3 }, //Maseru
	{"LT", 19540, "Lithuania", 1 }, //Vilnius
	{"LU", 19541, "Luxembourg", 1 }, //Luxembourg
	{"LV", 19542, "Latvia", 1 }, //Riga
	{"LY", 19545, "Libya", 3 }, //Tripoli
	{"MA", 19777, "Morocco", 3 }, //Casablanca
	{"MC", 19779, "Monaco", 1 }, //Monaco
	{"MD", 19780, "Moldova", 1 }, //Chisinau
	{"ME", 19781, "Montenegro", 1 }, //Podgorica
	{"MG", 19783, "Madagascar", 6 }, //Antananarivo
	{"MH", 19784, "Marshall Islands", 5 }, //Majuro
	{"MK", 19787, "Macedonia", 1 }, //Skopje
	{"ML", 19788, "Mali", 3 }, //Bamako
	{"MM", 19789, "Burma", 2 }, //Rangoon
	{"MN", 19790, "Mongolia", 2 }, //Ulaanbaatar
	{"MP", 19792, "Northern Mariana Islands", 5 }, //Saipan
	{"MQ", 19793, "Martinique", 4 }, //Martinique
	{"MR", 19794, "Mauritania", 3 }, //Nouakchott
	{"MT", 19796, "Malta", 1 }, //Malta
	{"MU", 19797, "Mauritius", 6 }, //Mauritius
	{"MV", 19798, "Maldives", 6 }, //Maldives
	{"MW", 19799, "Malawi", 3 }, //Blantyre
	{"MX", 19800, "Mexico", 4 }, //Mexico_City
	{"MY", 19801, "Malaysia", 2 }, //Kuala_Lumpur
	{"MZ", 19802, "Mozambique", 3 }, //Maputo
	{"NA", 20033, "Nairobi", 3 }, //Windhoek
	{"NC", 20035, "New Caledonia", 5 }, //Noumea
	{"NE", 20037, "Niger", 3 }, //Niamey
	{"NG", 20039, "Nigeria", 3 }, //Lagos
	{"NI", 20041, "Nicaragua", 4 }, //Managua
	{"NL", 20044, "Netherlands", 1 }, //Amsterdam
	{"NO", 20047, "Norway", 1 }, //Oslo
	{"NP", 20048, "Nepal", 2 }, //Katmandu
	{"NZ", 20058, "New Zealand", 5 }, //Auckland
	{"OM", 20301, "Oman", 2 }, //Muscat
	{"PA", 20545, "Panama", 4 }, //Panama
	{"PE", 20549, "Peru", 4 }, //Lima
	{"PF", 20550, "Tahiti", 5 }, //Tahiti
	{"PG", 20551, "Papua New Guinea", 5 }, //Port_Moresby
	{"PH", 20552, "Philippines", 2 }, //Manila
	{"PK", 20555, "Pakistan", 2 }, //Karachi
	{"PL", 20556, "Poland", 1 }, //Warsaw
	{"PM", 20557, "Miquelon", 4 }, //Miquelon
	{"PR", 20562, "Puerto Rico", 4 }, //Puerto_Rico
	{"PS", 20563, "Gaza Strip", 2 }, //Gaza
	{"PT", 20564, "Portugal", 1 }, //Lisbon
	{"PY", 20569, "Paraguay", 4 }, //Asuncion
	{"QA", 20801, "Qatar", 2 }, //Qatar
	{"RE", 21061, "Reunion", 6 }, //Reunion
	{"RO", 21071, "Romania", 1 }, //Bucharest
	{"RS", 21075, "Serbia", 1 }, //Belgrade
	{"RU", 21077, "Russia", 1 }, //Moscow
	{"RW", 21079, "Rwanda", 3 }, //Kigali
	{"SA", 21313, "Saudi Arabia", 2 }, //Riyadh
	{"SB", 21314, "Solomon Islands", 5 }, //Guadalcanal
	{"SC", 21315, "Mahe", 6 }, //Mahe
	{"SD", 21316, "Sudan", 3 }, //Khartoum
	{"SE", 21317, "Sweden", 1 }, //Stockholm
	{"SG", 21319, "Singapore", 2 }, //Singapore
	{"SI", 21321, "Slovenia", 1 }, //Ljubljana
	{"SK", 21323, "Slovakia", 1 }, //Bratislava
	{"SL", 21324, "Sierra Leone", 3 }, //Freetown
	{"SM", 21325, "San Marino", 1 }, //San_Marino
	{"SN", 21326, "Senegal", 3 }, //Dakar
	{"SO", 21327, "Somalia", 3 }, //Mogadishu
	{"SR", 21330, "Suriname", 4 }, //Paramaribo
	{"ST", 21332, "Sao Tome", 3 }, //Sao_Tome
	{"SV", 21334, "El Salvador", 4 }, //El_Salvador
	{"SY", 21337, "Syria", 2 }, //Damascus
	{"SZ", 21338, "Swaziland", 3 }, //Mbabane
	{"TD", 21572, "Chad", 3 }, //Ndjamena
	{"TG", 21575, "Togo", 3 }, //Lome
	{"TH", 21576, "Thailand", 2 }, //Bangkok
	{"TJ", 21578, "Tajikistan", 2 }, //Dushanbe
	{"TL", 21580, "Lesser Sunda Islands", 2 }, //Dili
	{"TM", 21581, "Turkmenistan", 2 }, //Ashgabat
	{"TN", 21582, "Tunis", 3 }, //Tunis
	{"TR", 21586, "Turkey", 1 }, //Istanbul
	{"TT", 21588, "Trinidad and Tobago", 4 }, //Port_of_Spain
	{"TW", 21591, "Taiwan", 2 }, //Taipei
	{"TZ", 21594, "Tanzania", 3 }, //Dar_es_Salaam
	{"UA", 21825, "Ukraine", 1 }, //Kiev
	{"UG", 21831, "Uganda", 3 }, //Kampala
	{"US", 21843, "United States of America", 4 }, //Indiana/Knox
	{"UY", 21849, "Uruguay", 4 }, //Montevideo
	{"UZ", 21850, "Uzbekistan", 2 }, //Samarkand
	{"VC", 22083, "Saint Vincent", 4 }, //St_Vincent
	{"VE", 22085, "Venezuela", 4 }, //Caracas
	{"VG", 22087, "Virgin Islands", 4 }, //Tortola
	{"VN", 22094, "Vietnam", 2 }, //Saigon
	{"VU", 22101, "Vanuatu", 5 }, //Efate
	{"WS", 22355, "Samoa", 5 }, //Apia
	{"YE", 22853, "Yemen", 2 }, //Aden
	{"YT", 22868, "Mayotte", 6 }, //Mayotte
	{"ZA", 23105, "South Africa", 3 }, //Johannesburg
	{"ZM", 23117, "Zambia", 3 }, //Lusaka
	{"ZW", 23127, "Zimbabwe", 3 }, //Harare
};

const char * TCountry::GetCountryName(UInt16 w)
{
	int i, j, mid;

	i = 0;
	j = sizeof(gcountries) / sizeof(TCountry) - 1;

	while(j > i)
	{
		if (gcountries[i].code == w)
			return gcountries[i].pszName;
		if (gcountries[j].code == w)
			return gcountries[i].pszName;
		mid = (i + j) / 2;
		if (gcountries[mid].code > w)
			j = mid;
		else
			i = mid;
	}

	return "";
}

const char * TCountry::GetCountryContinentName(UInt16 w)
{
	int i, j, mid;

	i = 0;
	j = sizeof(gcountries) / sizeof(TCountry) - 1;

	while(j > i)
	{
		if (gcountries[i].code == w)
			return gcontinents[gcountries[i].continent];
		if (gcountries[j].code == w)
			return gcontinents[gcountries[i].continent];
		mid = (i + j) / 2;
		if (gcountries[mid].code > w)
			j = mid;
		else
			i = mid;
	}

	return "";
}

int TCountry::GetCountryCount()
{
	return sizeof(gcountries) / sizeof(TCountry);
}

const char * TCountry::GetCountryNameByIndex(int nIndex)
{
	return gcountries[nIndex].pszName;
}

const char * TCountry::GetCountryContinentNameByIndex(int nIndex)
{
	return gcontinents[gcountries[nIndex].continent];
}

const char * TCountry::GetCountryAcronymByIndex(int nIndex)
{
	return gcountries[nIndex].pszAcr;
}
