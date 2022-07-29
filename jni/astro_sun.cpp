
#include "math.h"
#include "gmath.h"
#include "astro.h"
#include "stdlib.h"

//double SwissGetMoonLongitude(double jdate);
//double SwissGetSunLongitude(double jdate);
// PORTABLE 

/*************************************/
/*    sun functions                  */
/*                                   */
/*    sun longitude                  */
/*    sunrise, sunset time           */
/*                                   */
/*************************************/

double SunGetPerigee(int year, int month, int day);
double SunGetMeanLong(int year, int month, int day);

//
// takes values year, month, day, shour, TimeZone
//
double GetSunLongitude(VCTIME vct)
{
//	double mel = 0.0;
	double DG = pi / 180;
	double RAD = 180 / pi;

	// mean ecliptic longitude of the sun 
	double mel = SunGetMeanLong(vct.year, vct.month, vct.day) + (360/365.25)*(vct.shour - 0.5 - vct.tzone/24.0);

	// ecliptic longitude of perigee 
	double elp = SunGetPerigee(vct.year, vct.month, vct.day);

	// mean anomaly of the sun
	double mas = mel - elp;

	// ecliptic longitude of the sun
	//double els = 0.0;
	return  mel + 1.915 * sin(mas * DG) + 0.02 * sin (2 * DG * mas);

	/*double tmpl, x;
	double DayHours = date.shour - date.TimeZone/24.0;
	VCTIME tct;
	
	if (DayHours < 0.5)
	{
		tct = date;
		tct.PreviousDay();
		tmpl = SunGetMeanLong(tct.year, tct.month, tct.day); 
		mel = SunGetMeanLong(date.year, date.month, date.day);
		x = 0.5 + DayHours;
		mel = tmpl * (1-x) + mel * x;
	}
	else
	{
		tct = date;
		tct.NextDay();
		mel = SunGetMeanLong(tct.year, tct.month, tct.day);
		tmpl = SunGetMeanLong(date.year, date.month, date.day);
		x = DayHours - 0.5;
		mel = tmpl * (1-x) + mel * x;
	}

	//mel = SunGetMeanLong(date.year, date.month, date.day);

	// ecliptic longitude of perigee 
	double elp = SunGetPerigee(date.year, date.month, date.day);

	// mean anomaly of the sun in rads
	double mas = (mel - elp) * pi / 180.0;

	// ecliptic longitude of the sun
	return (mel + 1.915 * sin(mas) + 0.02 * sin (2 * mas));
*/
}

////////////////////////////////////////////////////////////////////
// find mean ecliptic longitude of the sun for your chosen day
//

double SunGetMeanLong(int year, int month, int day)
{
/*	VCTIME date;

	date.year = year;
	date.month = month;
	date.day = day;
	date.shour = hours;
	date.TimeZone = timezone;
	date.ChangeTimeZone(0.0);
*/
	static double sun_long[] = {
		339.226,009.781,039.351,069.906,099.475,130.030,160.585,190.155,220.710,250.279,280.834,311.390,
		340.212,010.767,040.337,070.892,100.461,131.016,161.571,191.141,221.696,251.265,281.820,312.375,
		341.198,011.753,041.322,071.877,101.447,132.002,162.557,192.126,222.681,252.251,282.806,313.361,
		342.183,012.738,042.308,072.863,102.432,132.987,163.542,193.112,223.667,253.236,283.791,314.346,
		343.169,013.724,043.293,073.849,103.418,133.973,164.528,194.098,224.653,254.222,284.777,315.332,
		344.155,014.710,044.279,074.834,104.404,134.959,165.514,195.083,225.638,255.208,285.763,316.318,
		345.140,015.695,045.265,075.820,105.389,135.944,166.499,196.069,226.624,256.193,286.748,317.303,
		346.126,016.681,046.250,076.805,106.375,136.930,167.485,197.054,227.610,257.179,287.734,318.289,
		347.112,017.667,047.236,077.791,107.361,137.916,168.471,198.040,228.595,258.165,288.720,319.275,
		348.097,018.652,048.222,078.777,108.346,138.901,169.456,199.026,229.581,259.150,289.705,320.260,
		349.083,019.638,049.207,079.762,109.332,139.887,170.442,200.011,230.566,260.136,290.691,321.246,
		350.068,020.624,050.193,080.748,110.317,140.873,171.428,200.997,231.552,261.122,291.677,322.232,
		351.054,021.609,051.179,081.734,111.303,141.858,172.413,201.983,232.538,262.107,292.662,323.217,
		352.040,022.595,052.164,082.719,112.289,142.844,173.399,202.968,233.523,263.093,293.648,324.203,
		353.025,023.581,053.150,083.705,113.274,143.829,174.385,203.954,234.509,264.078,294.634,325.189,
		354.011,024.566,054.136,084.691,114.260,144.815,175.370,204.940,235.495,265.064,295.619,326.174,
		354.997,025.552,055.121,085.676,115.246,145.801,176.356,205.925,236.480,266.050,296.605,327.160,
		355.982,026.537,056.107,086.662,116.231,146.786,177.341,206.911,237.466,267.035,297.590,328.146,
		356.968,027.523,057.093,087.648,117.217,147.772,178.327,207.897,238.452,268.021,298.576,329.131,
		357.954,028.509,058.078,088.633,118.203,148.758,179.313,208.882,239.437,269.007,299.562,330.117,
		358.939,029.494,059.064,089.619,119.188,149.743,180.298,209.868,240.423,269.992,300.547,331.102,
		359.925,030.480,060.049,090.605,120.174,150.729,181.284,210.854,241.409,270.978,301.533,332.088,
		000.911,031.466,061.035,091.590,121.160,151.715,182.270,211.839,242.394,271.964,302.519,333.074,
		001.896,032.451,062.021,092.576,122.145,152.700,183.255,212.825,243.380,272.949,303.504,334.059,
		002.882,033.437,063.006,093.561,123.131,153.686,184.241,213.810,244.366,273.935,304.490,335.045,
		003.868,034.423,063.992,094.547,124.117,154.672,185.227,214.796,245.351,274.921,305.476,336.031,
		004.853,035.408,064.978,095.533,125.102,155.657,186.212,215.782,246.337,275.906,306.461,337.016,
		005.839,036.394,065.963,096.518,126.088,156.643,187.198,216.767,247.322,276.892,307.447,338.002,
		006.824,037.380,066.949,097.504,127.073,157.629,188.184,217.753,248.308,277.878,308.433,338.988,
		007.810,038.365,067.935,098.490,128.059,158.614,189.169,218.739,249.294,278.863,309.418,339.100,
		008.796,038.365,068.920,098.490,129.045,159.600,189.169,219.724,249.294,279.849,310.404,339.226,
		};

	static double sun_1_col[] = {-001.157,-000.386,000.386,001.157};
	static double sun_1_row[] = {-001.070, 002.015, 005.101, 008.186,
		011.271, 014.356, 017.441, 020.526, 023.611, 026.697};
	static double sun_2_col[] = {000.322,000.107,-000.107,-000.322};
	static double sun_2_row[] = {-000.577,-000.449,-000.320,-000.192,
		-000.064, 000.064, 000.192, 000.320, 000.449, 000.577};
	double sun_3_row[] = {
		-000.370,-000.339,-000.309,-000.278,-000.247,-000.216,-000.185,-000.154,
		-000.123,-000.093,-000.062,-000.031,+000.000,+000.031,+000.062,+000.093,
		+000.123,+000.154,+000.185,+000.216,+000.247,+000.278,+000.309,+000.339,
		+000.370 };
	double sun_3_col[] = { +000.358,+000.119,-000.119,-000.358};

	double mel = 0.0;

	if ((month > 12) || (month < 1) || (day < 1) || (day > 31)) return -1.0;
	mel = sun_long[(day - 1)*12 + (month + 9)%12];

	int y, yy;
	
	if (month < 3)
	{
		y = abs((year - 1) / 100);
		yy = abs((year - 1) % 100);
	}
	else
	{
		y = abs(year / 100);
		yy = abs(year % 100);
	}

	if (y <= 15)
	{
		mel += sun_1_col[y%4] + sun_1_row[y/4];
	}
	else if (y < 40)
	{
		mel += sun_2_col[y%4] + sun_2_row[y/4];
	}


	mel += sun_3_col[yy%4] + sun_3_row[yy/4];

/*	static double sun_longitude[] = {
		338.240, 339.226, 340.212, 341.198, 342.183, 343.169, 344.155, 345.140, 346.126, 347.112, 
		348.097, 349.083, 350.068, 351.054, 352.040, 353.025, 354.011, 354.997, 355.982, 
		356.968, 357.954, 358.939, 359.925, 0.911, 1.896, 2.882, 3.868, 4.853, 5.839, 
		6.824, 7.810, 8.796, 9.781, 10.767, 11.753, 12.738, 13.724, 14.710, 
		15.695, 16.681, 17.667, 18.652, 19.638, 20.624, 21.609, 22.595, 23.581, 
		24.566, 25.552, 26.537, 27.523, 28.509, 29.494, 30.480, 31.466, 32.451, 
		33.437, 34.423, 35.408, 36.394, 37.380, 38.365, 38.365, 39.351, 40.337, 
		41.322, 42.308, 43.293, 44.279, 45.265, 46.250, 47.236, 48.222, 49.207, 
		50.193, 51.179, 52.164, 53.150, 54.136, 55.121, 56.107, 57.093, 58.078, 
		59.064, 60.049, 61.035, 62.021, 63.006, 63.992, 64.978, 65.963, 66.949, 
		67.935, 68.920, 69.906, 70.892, 71.877, 72.863, 73.849, 74.834, 75.820, 
		76.805, 77.791, 78.777, 79.762, 80.748, 81.734, 82.719, 83.705, 84.691, 
		85.676, 86.662, 87.648, 88.633, 89.619, 90.605, 91.590, 92.576, 93.561, 
		94.547, 95.533, 96.518, 97.504, 98.490, 98.490, 99.475, 100.461, 101.447, 
		102.432, 103.418, 104.404, 105.389, 106.375, 107.361, 108.346, 109.332, 110.317, 
		111.303, 112.289, 113.274, 114.260, 115.246, 116.231, 117.217, 118.203, 119.188, 
		120.174, 121.160, 122.145, 123.131, 124.117, 125.102, 126.088, 127.073, 128.059, 
		129.045, 130.030, 131.016, 132.002, 132.987, 133.973, 134.959, 135.944, 136.930, 
		137.916, 138.901, 139.887, 140.873, 141.858, 142.844, 143.829, 144.815, 145.801, 
		146.786, 147.772, 148.758, 149.743, 150.729, 151.715, 152.700, 153.686, 154.672, 
		155.657, 156.643, 157.629, 158.614, 159.600, 160.585, 161.571, 162.557, 163.542, 
		164.528, 165.514, 166.499, 167.485, 168.471, 169.456, 170.442, 171.428, 172.413, 
		173.399, 174.385, 175.370, 176.356, 177.341, 178.327, 179.313, 180.298, 181.284, 
		182.270, 183.255, 184.241, 185.227, 186.212, 187.198, 188.184, 189.169, 189.169, 
		190.155, 191.141, 192.126, 193.112, 194.098, 195.083, 196.069, 197.054, 198.040, 
		199.026, 200.011, 200.997, 201.983, 202.968, 203.954, 204.940, 205.925, 206.911, 
		207.897, 208.882, 209.868, 210.854, 211.839, 212.825, 213.810, 214.796, 215.782, 
		216.767, 217.753, 218.739, 219.724, 220.710, 221.696, 222.681, 223.667, 224.653, 
		225.638, 226.624, 227.610, 228.595, 229.581, 230.566, 231.552, 232.538, 233.523, 
		234.509, 235.495, 236.480, 237.466, 238.452, 239.437, 240.423, 241.409, 242.394, 
		243.380, 244.366, 245.351, 246.337, 247.322, 248.308, 249.294, 249.294, 250.279, 
		251.265, 252.251, 253.236, 254.222, 255.208, 256.193, 257.179, 258.165, 259.150, 
		260.136, 261.122, 262.107, 263.093, 264.078, 265.064, 266.050, 267.035, 268.021, 
		269.007, 269.992, 270.978, 271.964, 272.949, 273.935, 274.921, 275.906, 276.892, 
		277.878, 278.863, 279.849, 280.834, 281.820, 282.806, 283.791, 284.777, 285.763, 
		286.748, 287.734, 288.720, 289.705, 290.691, 291.677, 292.662, 293.648, 294.634, 
		295.619, 296.605, 297.590, 298.576, 299.562, 300.547, 301.533, 302.519, 303.504, 
		304.490, 305.476, 306.461, 307.447, 308.433, 309.418, 310.404, 311.390, 312.375, 
		313.361, 314.346, 315.332, 316.318, 317.303, 318.289, 319.275, 320.260, 321.246, 
		322.232, 323.217, 324.203, 325.189, 326.174, 327.160, 328.146, 329.131, 330.117, 
		331.102, 332.088, 333.074, 334.059, 335.045, 336.031, 337.016, 338.002, 338.988, 
		339.974, 340.960
	};

	static double sun_1_col[] = {-001.157,-000.386,000.386,001.157};
	static double sun_1_row[] = {-001.070, 002.015, 005.101, 008.186,
		011.271, 014.356, 017.441, 020.526, 023.611, 026.697};
	static double sun_2_col[] = {000.322,000.107,-000.107,-000.322};
	static double sun_2_row[] = {-000.577,-000.449,-000.320,-000.192,
		-000.064, 000.064, 000.192, 000.320, 000.449, 000.577};
	double sun_3_row[] = {
		-000.370,-000.339,-000.309,-000.278,-000.247,-000.216,-000.185,-000.154,
		-000.123,-000.093,-000.062,-000.031,+000.000,+000.031,+000.062,+000.093,
		+000.123,+000.154,+000.185,+000.216,+000.247,+000.278,+000.309,+000.339,
		+000.370 };
	double sun_3_col[] = { +000.358,+000.119,-000.119,-000.358};

	double mel = 0.0;

	if ((date.month > 12) || (date.month < 1) || (date.day < 1) || (date.day > 31))
		return -1.0;

	int index;
	double coef, e;

	index = ((date.month + 9)%12) * 31 + date.day;

	if (date.shour < 0.5)
	{
		coef = 0.5 - date.shour;
		e = sun_longitude[index-1];
		if (e > sun_longitude[index])
			e -= 360.0;
		mel = coef * e + (1 - coef) * sun_longitude[index];
	}
	else
	{
		coef = date.shour - 0.5;
		e = sun_longitude[index];
		if (e > sun_longitude[index+1])
			e -= 360.0;
		mel = (1 - coef) * e + coef * sun_longitude[index+1];
	}

	int y, yy;
	
	if (date.month < 3)
	{
		y = (date.year - 1) / 100;
		yy = (date.year - 1) % 100;
	}
	else
	{
		y = date.year / 100;
		yy = date.year % 100;
	}

	if (y <= 15)
	{
		mel += sun_1_col[y%4] + sun_1_row[y/4];
	}
	else if (y < 40)
	{
		mel += sun_2_col[y%4] + sun_2_row[y/4];
	}


	mel += sun_3_col[yy%4] + sun_3_row[yy/4];*/

	return mel;
}


////////////////////////////////////////////////////////////////
// finds ecliptic longitude of perigee of the sun 
// for the mean summer solstice of your chosen year 
// (and effectively for the entire year)

double SunGetPerigee(int year, int month, int day)
{
	static double sun_4_row[] = {251.97,258.85,265.73,272.61,279.49
		,286.37,293.25,300.14,307.02,313.90};
	static double sun_4_col[] = {-002.58,-000.86,000.86,002.58 };
	static double sun_5_row[] = {-000.83,-000.76,-000.69,-000.62,
		-000.55,-000.48,-000.41,-000.34,
		-000.28,-000.21,-000.14,-000.07,
		+000.00,+000.07,+000.14,+000.21,
		+000.28,+000.34,+000.41,+000.48,
		+000.55,+000.62,+000.69,+000.76,
		+000.83 };
	static double sun_5_col[] = {-000.03,-000.01,000.01,+000.03};

	double per = 0.0;

	if ((month > 12) || (month < 1) || (day < 1) || (day > 31)) return -1.0;

	int y, yy;
	
	if (month < 3)
	{
		y = abs((year - 1) / 100);
		yy = abs((year - 1) % 100);
	}
	else
	{
		y = abs(year / 100);
		yy = abs(year % 100);
	}

	per = sun_4_row[y / 4] + sun_4_col[y % 4];
	per += sun_5_row[yy/4] + sun_5_col[yy%4];

	return per;
}


//////////////////////////////////////////////////////////////////////////
//
// VCTIME vct [in] - valid must be each member of this structure
//

// from vct uses members: year, month, day
// DayHours is in range 0.0 - 1.0

void SunPosition(VCTIME vct, EARTHDATA ed, SUNDATA &sun, DAYHOURS DayHours)
{
	double DG = pi / 180;
	double RAD = 180 / pi;

	double x;

	double dLatitude = ed.latitude_deg;
	double dLongitude = ed.longitude_deg;

	// mean ecliptic longitude of the sun 
	double mel = SunGetMeanLong(vct.year, vct.month, vct.day) + (360/365.25)*DayHours/360.0;

	// ecliptic longitude of perigee 
	double elp = SunGetPerigee(vct.year, vct.month, vct.day);

	// mean anomaly of the sun
	double mas = mel - elp;

	// ecliptic longitude of the sun
	double els = 0.0;
	sun.longitude_deg = els = mel + 1.915 * sin(mas * DG) + 0.02 * sin (2 * DG * mas);

	// declination of the sun
	sun.declination_deg = RAD * asin (0.39777 * sin(els * DG));

	// right ascension of the sun
	sun.right_asc_deg = els - RAD * atan2( sin(2*els*DG), 23.2377 + cos(2 * DG * els));

	// equation of time
	double eqt = 0.0;
	eqt = sun.right_asc_deg - mel;


	// definition of event
	double eventdef = 0.01454;
/*	switch(ed.obs)
	{
	case 1:	// civil twilight
		eventdef = 0.10453;
		break;
	case 2:	// nautical twilight
		eventdef = 0.20791;
		break;
	case 3:	// astronomical twilight
		eventdef = 0.30902;
		break;
	default:// center of the sun on the horizont
		eventdef = 0.01454;
		break;
	}*/

	eventdef = (eventdef / cos(dLatitude * DG)) / cos(sun.declination_deg * DG);

	x = tan(dLatitude * DG) * tan(sun.declination_deg * DG) + eventdef;

	// initial values for the case
	// that no rise no set for that day
	sun.sunrise_deg = sun.sunset_deg = -360.0;

	if ((x >= -1.0) && (x <= 1.0))
	{
		// time of sunrise
		sun.sunrise_deg = 90.0 - dLongitude - RAD * asin(x) + eqt;

		// time of sunset
		sun.sunset_deg = 270.0 - dLongitude + RAD * asin(x) + eqt;
	}



}


//////////////////////////////////////////////////////////////////////////////////
//
// return values are in sun.arunodaya, sun.rise, sun.set, sun.noon, sun.length
// if values are less than zero, that means, no sunrise, no sunset in that day
//
// brahma 1 = calculation at brahma muhurta begining
// brahma 0 = calculation at sunrise


void SunCalc(VCTIME vct, EARTHDATA earth, SUNDATA &sun)
{
/*	double timeZone = vct.TimeZone;
	double sunset = 0.0;

	// calculation SUNSET
	SunPosition(vct, earth, sun, 0.0);

	if (sun.sunset_deg >= 0.0)
	{
		// second calculation
		// for time of sunrise
		SunPosition(vct, earth, sun, sun.sunset_deg / 360.0 );

		// third (last) calculation
		// for time of sunrise
		SunPosition(vct, earth, sun, sun.sunset_deg / 360.0 );

		// time in degree for SUNSET
		sunset = sun.sunset_deg;

		// longitude of Sun at SUNSET
		sun.longitude_set_deg = sun.longitude_deg;
	}

	// calculation SUNRISE
	// first calculation
	// for 12:00 universal time
	SunPosition(vct, earth, sun, 0.0);

	if (sun.sunrise_deg >= 0.0)
	{
		// second calculation
		// for time of sunrise
		SunPosition(vct, earth, sun, sun.sunrise_deg / 360.0);

		// third (last) calculation
		// for time of sunrise
		SunPosition(vct, earth, sun, sun.sunrise_deg / 360.0);
	}
	// calculate times

	//  sunrise_deg is from range 0-360 so 96min=24deg
	// arunodaya is 96 min before sunrise
	SUNDATA sun2;
	SunPosition(vct, earth, sun2, (sun.sunrise_deg - 24.0)/360.0);
	sun.longitude_arun_deg = sun2.longitude_deg;

	// time in degree for SUNSET
	sun.sunset_deg = sunset;
	
	sun.arunodaya.SetDegTime(sun.sunrise_deg - 24.0 + timeZone*15.0);
	// sunrise
	sun.rise.SetDegTime(sun.sunrise_deg + timeZone*15.0);
	// noon
	sun.noon.SetDegTime((sun.sunset_deg + sun.sunrise_deg)/2  + timeZone*15.0);
	// sunset
	sun.set.SetDegTime(sun.sunset_deg + timeZone*15.0);
	// length
	sun.length.SetDegTime(sun.sunset_deg - sun.sunrise_deg);

*/
	SUNDATA s_rise, s_set;

	// first calculation
	// for 12:00 universal time
	SunPosition(vct, earth, s_rise, 0.0);
	// second calculation
	// for time of sunrise
	SunPosition(vct, earth, s_rise, s_rise.sunrise_deg - 180);
	// third (last) calculation
	// for time of sunrise
	SunPosition(vct, earth, s_rise, s_rise.sunrise_deg - 180);
	// first calculation
	// for 12:00 universal time
	SunPosition(vct, earth, s_set, 0.0);
	// second calculation
	// for time of sunrise
	SunPosition(vct, earth, s_set, s_set.sunset_deg - 180);
	// third (last) calculation
	// for time of sunrise
	SunPosition(vct, earth, s_set, s_set.sunset_deg - 180);

	// calculate times
	sun.longitude_arun_deg = s_rise.longitude_deg - (24.0 / 365.25);
	sun.longitude_deg = s_rise.longitude_deg;
	sun.longitude_set_deg = s_set.longitude_deg;

	sun.arunodaya_deg = s_rise.sunrise_deg - 24.0;
	sun.sunrise_deg = s_rise.sunrise_deg;
	sun.sunset_deg = s_set.sunset_deg;
	sun.length_deg = s_set.sunset_deg - s_rise.sunrise_deg;

	// arunodaya is 96 min before sunrise
	//  sunrise_deg is from range 0-360 so 96min=24deg
	sun.arunodaya.SetDegTime(sun.arunodaya_deg + earth.tzone*15.0);
	// sunrise
	sun.rise.SetDegTime(sun.sunrise_deg + earth.tzone*15.0);
	// noon
	sun.noon.SetDegTime((sun.sunset_deg + sun.sunrise_deg)/2  + earth.tzone*15.0);
	// sunset
	sun.set.SetDegTime(sun.sunset_deg + earth.tzone*15.0);
	// length
	sun.length.SetDegTime(sun.length_deg);

}

void TestFunc(void)
{
	double d = 0.0;

	double dayHours = 0.5;
	EARTHDATA ed;
	ed.latitude_deg = 17.0;
	ed.longitude_deg = 45.0;
	ed.tzone = 1.0;
	VCTIME vct;
	vct.day = 5;
	vct.month = 10;
	vct.year = 2010;
	vct.tzone = 1.0;

//	SunCalc(vct, ed, sun);

	printf("%f", d);
}


void calc_epsilon_phi(JULIANDATE, double &, double &);

/*********************************************************************/
/*                                                                   */
/* Value of Ayanamsha for given Julian day                           */
/*                                                                   */
/*********************************************************************/

/*
27.8.1900     22-27-54  2415259.000000 22,475
23.7.1950     23-09-53  2433486.000000 23,16472
3.9.2000      23-52-13  2451791.000000 23,870277778
28.8.2010     24-00-04  2455437.000000 24,001111111
21.6.2050     24-33-29  2469979.000000 24,558055556
14.6.2100     25-15-29  2488234.000000 25,258055556*/

double GetLahiriAyanamsa(JULIANDATE d)
{
	double h[] = { 2415259.000000,22.475,
		2433486.000000,23.16472,
		2451791.000000,23.870277778,
		2455437.000000,24.001111111,
		2469979.000000,24.558055556,
		2488234.000000,25.258055556 };

	if (d > h[10])
	{
		return (d - h[10]) * ((h[11] - h[9]) / (h[10] - h[8])) + h[11];
	}
	else if (d > h[8])
	{
		return (d - h[8]) * ((h[11] - h[9]) / (h[10] - h[8])) + h[9];
	}
	else if (d > h[6])
	{
		return (d - h[6]) * ((h[9] - h[7]) / (h[8] - h[6])) + h[7];
	}
	else if (d > h[4])
	{
		return (d - h[4]) * ((h[7] - h[5]) / (h[6] - h[4])) + h[5];
	}
	else if (d > h[2])
	{
		return (d - h[2]) * ((h[5] - h[3]) / (h[4] - h[2])) + h[3];
	}
	else if (d > h[0])
	{
		return (d - h[0]) * ((h[3] - h[1]) / (h[2] - h[0])) + h[1];
	}
	else
	{
		return (d - h[0]) * ((h[3] - h[1]) / (h[2] - h[0])) + h[1];
	}
}

//==================================================================
//
//==================================================================

extern int gAyaType;
extern int gSanType;

//==================================================================
//
//==================================================================

int GetAyanamsaType(void)
{
	return gAyaType;
}

//==================================================================
//
//==================================================================

int SetAyanamsaType(int i)
{
	int prev = gAyaType;
	gAyaType = i;
	return prev;
}

//==================================================================
//
//==================================================================

int GetSankrantiType(void)
{
	return gSanType;
}

//==================================================================
//
//==================================================================

int SetSankrantiType(int i)
{
	int prev = gSanType;
	gSanType = i;
	return prev;
}

//==================================================================
//
//==================================================================

const char * GetAyanamsaName(int nType)
{
	const char * pNames[] = {
		"Fagan/Bradley",
		"Lahiri",
		"Krishnamurti",
		"Raman"
	};

	if (nType > 3)
		return "";

	return pNames[nType];
}

const char * GetSankMethodName(int i)
{
	static const char * snam[] = {
		"midnight to midnight",
		"sunrise to sunrise",
		"noon to noon",
		"sunset to sunset"};

	return snam[i];
}

//==================================================================
//
// precession of the equinoxes http://en.wikipedia.org/wiki/Precession_%28astronomy%29
//
//==================================================================

double GetAyanamsa(JULIANDATE jdate)
{
	double t, d;
	double a1;


	// progress of ayanamsa from 1950 to 2000
//1.3971948971667
	t = (jdate - 2451545.0) / 36525.0;
	d = (5028.796195 - 1.1054348*t)*t / 3600.0;
	switch(gAyaType)
	{
		case 0: // Fagan-Bradley
			a1 = 24.8361111111 -0.095268987143399+ d; 
			//-69.943382314 + jdate * 3.8263328316687189e-5;
			break;
		case 1: // Lahiri
			a1 = 23.85305555555 + d;
			//a1 = GetLahiriAyanamsa(jdate);
			// 23-51-14 in 2000
			//TRACE("Ayan lahiri = %f\n", a1 - GetLahiriAyanamsa(jdate));
			break;
		case 2: // Krishnamurti
			a1 = 23.8561111111 -0.095268987143399+ d;
			break;
		case 3: // Raman
			a1 = 22.5066666666 -0.095268987143399+ d;
			break;
		default:
			break;
	}

	return a1;

}

/*********************************************************************/
/*                                                                   */
/* Calculation of Rasi from sun-logitude and ayanamsa                */
/*                                                                   */
/*********************************************************************/

int GetRasi(double SunLongitude, double Ayanamsa)
{
	return (int)floor(put_in_360(SunLongitude - Ayanamsa)/30.0);
}

/*********************************************************************/
/*  Finds next time when rasi is changed                             */
/*                                                                   */
/*  startDate - starting date and time, timezone member must be valid */
/*  zodiac [out] - found zodiac sign into which is changed           */
/*                                                                   */
/*********************************************************************/

VCTIME GetNextSankranti( VCTIME startDate, int &zodiac)
{
	VCTIME d;
	double step = 1.0;
	int count = 0;
	double ld, prev;
	int prev_rasi, new_rasi;
	VCTIME prevday;

	d = startDate;
	//d.ChangeTimeZone(0.0);
	//d.shour = 0.0;

	prev = put_in_360( GetSunLongitude(d) - GetAyanamsa(d.GetJulian()));
	prev_rasi = (int)floor(prev / 30.0);

	while(count < 20)
	{
		prevday = d;
		d.shour += step;
		if (d.shour > 1.0)
		{
			d.shour -= 1.0;
			d.NextDay();
		}
		
		ld = put_in_360(GetSunLongitude(d) - GetAyanamsa(d.GetJulian()));
		new_rasi = (int)floor(ld/30.0);

		if (prev_rasi != new_rasi)
		{
			zodiac = new_rasi;
			//v uplynulom dni je sankranti
			step *= 0.5;
			d = prevday;
			count++;
			continue;
		}
	}

	return d;
}
