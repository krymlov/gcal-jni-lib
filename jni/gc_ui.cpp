#include "astro.h"
#include "avc.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

// PORTABLE

const char * GetTithiName(int i)
{
	return gstr[600 + i % 30];
}

const char * GetNaksatraName(int n)
{
	return gstr[630 + n % 27];

}

const char * GetNaksatraChildSylable(int n, int pada)
{
	int i = (n * 4 + pada) % 108;

	static char * childsylable[108] = {
		"chu", "che", "cho", "la", //asvini
		"li", "lu", "le", "lo", // bharani
		"a", "i", "u", "e", //krtika
		"o", "va", "vi", "vo", // rohini
		"ve", "vo", "ka", "ke", // mrgasira
		"ku","gha","ng","chha", // ardra
		"ke","ko","ha","hi", // punarvasu
		"hu","he","ho","da", // pushya
		"di","du","de","do", //aslesa
		"ma","mi","mu","me", //magha
		"mo","ta","ti","tu", //purvaphalguni
		"te","to","pa","pi", //uttaraphalguni
		"pu","sha","na","tha",//hasta
		"pe","po","ra","ri",//chitra
		"ru","re","ra","ta",//svati
		"ti","tu","te","to",//visakha
		"na","ni","nu","ne",// anuradha
		"no","ya","yi","yu",//jyestha
		"ye","yo","ba","bi",// mula
		"bu","dha","bha","dha",//purvasada
		"be","bo","ja","ji",// uttarasada
		"ju","je","jo","gha",//sravana
		"ga","gi","gu","ge",// dhanistha
		"go","sa","si","su",//satabhisda
		"se","so","da","di",//purvabhadra
		"du","tha","jna","da",//uttarabhadra
		"de","do","cha","chi"// revati

	};

	return childsylable[i];
}

const char * GetRasiChildSylable(int n)
{
	static char * childsylable[12] = {
		"a... la",
		"u... va... i... e... o",
		"ka... cha... gha",
		"ha",
		"ma",
		"pa",
		"ra... ta",
		"na... ba",
		"dha... bha... pha",
		"kha... ja",
		"ga... sa",
		"da... ca... jha"
	};
	/*Mesa :	 	 a   la
Vrsabha: 	   u   va,   i, e, o
Mithuna:  	 ka,  cha, gha
Kataka:	   ha
Simha:		  ma
Kanya:		 pa
Tula:		 ra , ta
Vrschika:	  na ,  ba

Dhanus:	   dha , bha,  pha
Makra:  	kha, ja
Kumbha:	   ga,, sa
Mina:		  da, ca , jha
*/		

	return childsylable[n % 12];
}

const char * GetYogaName(int n)
{
	return gstr[660 + n % 27];
}

const char * GetSankrantiName(int i)
{
	return gstr[688 + i % 12];
}

const char * GetSankrantiNameEn(int i)
{

	return gstr[700 + i % 12];
}

char GetPaksaChar(int i)
{
	return (i ? 'G' : 'K');
}

const char * GetPaksaName(int i)
{
	return (i ? gstr[712] : gstr[713]);
}

const char * GetMasaName(int i)
{
	return gstr[720 + i % 13];

}

const char * GetMahadvadasiName(int i)
{
	switch(i)
	{
	case EV_NULL:
	case EV_SUDDHA:
		return NULL;
	case EV_UNMILANI:
		return gstr[733];
	case EV_TRISPRSA:
	case EV_UNMILANI_TRISPRSA:
		return gstr[734];
	case EV_PAKSAVARDHINI:
		return gstr[735];
	case EV_JAYA:
		return gstr[736];
	case EV_VIJAYA:
		return gstr[737];
	case EV_PAPA_NASINI:
		return gstr[738];
	case EV_JAYANTI:
		return gstr[739];
	case EV_VYANJULI:
		return gstr[740];
	default:
		return NULL;
	}
}

const char * GetSpecFestivalName(int i)
{
	switch(i)
	{
	case SPEC_JANMASTAMI:
		return gstr[741];
	case SPEC_GAURAPURNIMA:
		return gstr[742];
	case SPEC_RETURNRATHA:
		return gstr[743];
	case SPEC_HERAPANCAMI:
		return gstr[744];
	case SPEC_GUNDICAMARJANA:
		return gstr[745];
	case SPEC_GOVARDHANPUJA:
		return gstr[746];
	case SPEC_RAMANAVAMI:
		return gstr[747];
	case SPEC_RATHAYATRA:
		return gstr[748];
	case SPEC_NANDAUTSAVA:
		return gstr[749];
	case SPEC_PRABHAPP:
		return gstr[759];
	case SPEC_MISRAFESTIVAL:
		return gstr[750];
	default:
		return gstr[64];
	}
}

Boolean GetSpecFestivalNameEx(TString &str, int i)
{
	switch(i)
	{
	case SPEC_JANMASTAMI:
		str = gstr[741];
		str += "[f:5:Sri Krsna]";
		break;
	case SPEC_GAURAPURNIMA:
		str = gstr[742];
		str += "[f:3:Sri Caitanya Mahaprabhu]";
		break;
	case SPEC_RETURNRATHA:
		str = gstr[743];
		break;
	case SPEC_HERAPANCAMI:
		str = gstr[744];
		break;
	case SPEC_GUNDICAMARJANA:
		str = gstr[745];
		break;
	case SPEC_GOVARDHANPUJA:
		str = gstr[746];
		break;
	case SPEC_RAMANAVAMI:
		str = gstr[747];
		str += "[f:1:Sri Ramacandra]";
		break;
	case SPEC_RATHAYATRA:
		str = gstr[748];
		break;
	case SPEC_NANDAUTSAVA:
		str = gstr[749];
		break;
	case SPEC_PRABHAPP:
		str = gstr[759];
		str += "[f:1:Srila Prabhupada]";
		break;
	case SPEC_MISRAFESTIVAL:
		str = gstr[750];
		break;
	default:
		str = gstr[64];
		return false;
	}

	return true;
}


const char * GetFastingName(int i)
{
	switch (i)
	{
	case FAST_NOON:
		return( gstr[751]);
		break;
	case FAST_SUNSET:
		return( gstr[752]);
		break;
	case FAST_MOONRISE:
		return( gstr[753]);
		break;
	case FAST_DUSK:
		return( gstr[754]);
		break;
	case FAST_MIDNIGHT:
		return( gstr[755]);
		break;
	default:
		return NULL;
	}
}

const char * GetEkadasiName(int nMasa, int nPaksa)
{
	return gstr[560 + nMasa*2 + nPaksa];
}

