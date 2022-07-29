
#include "gmath.h"
#include "math.h"
#include "astro.h"
#include "stdlib.h"

double SwissGetMoonLongitude(double jdate);
double SwissGetSunLongitude(double jdate);

// PORTABLE 
/*********************************************************************************/
/*  Procedures for calculation                                                   */
/*                                                                               */
/*    - moon longitude (MoonCalc)                                                */
/*    - moonrise time (CalcMoonTimes)                                            */
/*    - moonset time                                                             */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/


void correct_position(MOONDATA & position, JULIANDATE jdate, double latitude, double longitude, double height);


void calc_epsilon_phi(JULIANDATE date, double & delta_phi, double & epsilon)
{
	const int arg_mul[31][5] = {
		 { 0, 0, 0, 0, 1},
		 {-2, 0, 0, 2, 2},
		 { 0, 0, 0, 2, 2},
		 { 0, 0, 0, 0, 2},
		 { 0, 1, 0, 0, 0},
		 { 0, 0, 1, 0, 0},
		 {-2, 1, 0, 2, 2},
		 { 0, 0, 0, 2, 1},
		 { 0, 0, 1, 2, 2},
		 {-2,-1, 0, 2, 2},
		 {-2, 0, 1, 0, 0},
		 {-2, 0, 0, 2, 1},
		 { 0, 0,-1, 2, 2},
		 { 2, 0, 0, 0, 0},
		 { 0, 0, 1, 0, 1},
		 { 2, 0,-1, 2, 2},
		 { 0, 0,-1, 0, 1},
		 { 0, 0, 1, 2, 1},
		 {-2, 0, 2, 0, 0},
		 { 0, 0,-2, 2, 1},
		 { 2, 0, 0, 2, 2},
		 { 0, 0, 2, 2, 2},
		 { 0, 0, 2, 0, 0},
		 {-2, 0, 1, 2, 2},
		 { 0, 0, 0, 2, 0},
		 {-2, 0, 0, 2, 0},
		 { 0, 0,-1, 2, 1},
		 { 0, 2, 0, 0, 0},
		 { 2, 0,-1, 0, 1},
		 {-2, 2, 0, 2, 2},
		 { 0, 1, 0, 0, 1}
					   };
	const int arg_phi[31][2] = {
		 {-171996,-1742},
		 { -13187,  -16},
		 {  -2274,   -2},
		 {   2062,    2},
		 {   1426,  -34},
		 {    712,    1},
		 {   -517,   12},
		 {   -386,   -4},
		 {   -301,    0},
		 {    217,   -5},
		 {   -158,    0},
		 {    129,    1},
		 {    123,    0},
		 {     63,    0},
		 {     63,    1},
		 {    -59,    0},
		 {    -58,   -1},
		 {    -51,    0},
		 {     48,    0},
		 {     46,    0},
		 {    -38,    0},
		 {    -31,    0},
		 {     29,    0},
		 {     29,    0},
		 {     26,    0},
		 {    -22,    0},
		 {     21,    0},
		 {     17,   -1},
		 {     16,    0},
		 {    -16,    1},
		 {    -15,    0}
		};
	const int arg_eps[31][2] = {
		{ 92025,   89},
		{  5736,  -31},
		{   977,   -5},
		{  -895,    5},
		{    54,   -1},
		{    -7,    0},
		{   224,   -6},
		{   200,    0},
		{   129,   -1},
		{   -95,    3},
		{     0,    0},
		{   -70,    0},
		{   -53,    0},
		{     0,    0},
		{   -33,    0},
		{    26,    0},
		{    32,    0},
		{    27,    0},
		{     0,    0},
		{   -24,    0},
		{    16,    0},
		{    13,    0},
		{     0,    0},
		{   -12,    0},
		{     0,    0},
		{     0,    0},
		{   -10,    0},
		{     0,    0},
		{    -8,    0},
		{     7,    0},
		{     9,    0}
	};

	double t,omega;
	double d,m,ms,f,s, l, ls;
	int i;
	double epsilon_0,delta_epsilon;

	t = ( date -2451545.0)/36525;

	// longitude of rising knot
	omega =put_in_360(125.04452+(-1934.136261+(0.0020708+1.0/450000*t)*t)*t);

	if (1)
	{
 // (*@/// delta_phi and delta_epsilon - low accuracy *)
  //(* mean longitude of sun (l) and moon (ls) *)
  l = 280.4665+36000.7698*t;
  ls = 218.3165+481267.8813*t;

  //(* correction due to nutation *)
  delta_epsilon =9.20*cos_d(omega)+0.57*cos_d(2*l)+0.10*cos_d(2*ls)-0.09*cos_d(2*omega);

  //(* longitude correction due to nutation *)
  delta_phi =(-17.20*sin_d(omega)-1.32*sin_d(2*l)-0.23*sin_d(2*ls)+0.21*sin_d(2*omega))/3600;
	}
	else
	{
	// mean elongation of moon to sun
	d = put_in_360(297.85036+(445267.111480+(-0.0019142+t/189474)*t)*t);

	// mean anomaly of the sun
	m =put_in_360(357.52772+(35999.050340+(-0.0001603-t/300000)*t)*t);

	// mean anomaly of the moon
	ms =put_in_360(134.96298+(477198.867398+(0.0086972+t/56250)*t)*t);

	// argument of the latitude of the moon
	f = put_in_360(93.27191+(483202.017538+(-0.0036825+t/327270)*t)*t);

	delta_phi = 0;
	delta_epsilon = 0;

	for(i=0;i<31;i++)
	{
		s= arg_mul[i][0]*d
		   +arg_mul[i][1]*m
		   +arg_mul[i][2]*ms
		   +arg_mul[i][3]*f
		   +arg_mul[i][4]*omega;
		delta_phi=delta_phi+(arg_phi[i][0]+arg_phi[i][1]*t*0.1)*sin_d(s);
		delta_epsilon=delta_epsilon+(arg_eps[i][0]+arg_eps[i][1]*t*0.1)*cos_d(s);
	}

	delta_phi=delta_phi*0.0001/3600;
	delta_epsilon=delta_epsilon*0.0001/3600;
	}
	// angle of ecliptic
	epsilon_0=84381.448+(-46.8150+(-0.00059+0.001813*t)*t)*t;

	epsilon=(epsilon_0+delta_epsilon)/3600;

}



void calc_geocentric(double &longitude, double &latitude, double &rektaszension, double &declination, JULIANDATE date)
{
	//var
	double epsilon; //: extended;
	double delta_phi; //: extended;
	double alpha,delta; //: extended;

	calc_epsilon_phi(date, delta_phi, epsilon);
	longitude = put_in_360(longitude+delta_phi);

	alpha = arctan2_d( sin_d(longitude)*cos_d(epsilon)-tan_d(latitude)*sin_d(epsilon), cos_d(longitude));

	delta = arcsin_d( sin_d(latitude)*cos_d(epsilon)+cos_d(latitude)*sin_d(epsilon)*sin_d(longitude));

	rektaszension = alpha;
	declination = delta;

	double xg, yg, zg;

	xg = cos_d(longitude)*cos_d(latitude);
	yg = sin_d(longitude)*cos_d(latitude);
	zg = sin_d(latitude);

	alpha = arctan2_d(yg*cos_d(epsilon) - zg*sin_d(epsilon), cos_d(longitude)*cos_d(latitude));
}

void MoonCalc(JULIANDATE jdate, MOONDATA &moon, EARTHDATA earth)
{
	const int arg_lr[60][4] = {
		 { 0, 0, 1, 0},
		 { 2, 0,-1, 0},
		 { 2, 0, 0, 0},
		 { 0, 0, 2, 0},
		 { 0, 1, 0, 0},
		 { 0, 0, 0, 2},
		 { 2, 0,-2, 0},
		 { 2,-1,-1, 0},
		 { 2, 0, 1, 0},
		 { 2,-1, 0, 0},
		 { 0, 1,-1, 0},
		 { 1, 0, 0, 0},
		 { 0, 1, 1, 0},
		 { 2, 0, 0,-2},
		 { 0, 0, 1, 2},
		 { 0, 0, 1,-2},
		 { 4, 0,-1, 0},
		 { 0, 0, 3, 0},
		 { 4, 0,-2, 0},
		 { 2, 1,-1, 0},
		 { 2, 1, 0, 0},
		 { 1, 0,-1, 0},
		 { 1, 1, 0, 0},
		 { 2,-1, 1, 0},
		 { 2, 0, 2, 0},
		 { 4, 0, 0, 0},
		 { 2, 0,-3, 0},
		 { 0, 1,-2, 0},
		 { 2, 0,-1, 2},
		 { 2,-1,-2, 0},
		 { 1, 0, 1, 0},
		 { 2,-2, 0, 0},
		 { 0, 1, 2, 0},
		 { 0, 2, 0, 0},
		 { 2,-2,-1, 0},
		 { 2, 0, 1,-2},
		 { 2, 0, 0, 2},
		 { 4,-1,-1, 0},
		 { 0, 0, 2, 2},
		 { 3, 0,-1, 0},
		 { 2, 1, 1, 0},
		 { 4,-1,-2, 0},
		 { 0, 2,-1, 0},
		 { 2, 2,-1, 0},
		 { 2, 1,-2, 0},
		 { 2,-1, 0,-2},
		 { 4, 0, 1, 0},
		 { 0, 0, 4, 0},
		 { 4,-1, 0, 0},
		 { 1, 0,-2, 0},
		 { 2, 1, 0,-2},
		 { 0, 0, 2,-2},
		 { 1, 1, 1, 0},
		 { 3, 0,-2, 0},
		 { 4, 0,-3, 0},
		 { 2,-1, 2, 0},
		 { 0, 2, 1, 0},
		 { 1, 1,-1, 0},
		 { 2, 0, 3, 0},
		 { 2, 0,-1,-2}
	   };

	  const int arg_b[60][4] = {
		 { 0, 0, 0, 1},
		 { 0, 0, 1, 1},
		 { 0, 0, 1,-1},
		 { 2, 0, 0,-1},
		 { 2, 0,-1, 1},
		 { 2, 0,-1,-1},
		 { 2, 0, 0, 1},
		 { 0, 0, 2, 1},
		 { 2, 0, 1,-1},
		 { 0, 0, 2,-1}, // {* !!! Error in German Meeus *}
		 { 2,-1, 0,-1},
		 { 2, 0,-2,-1},
		 { 2, 0, 1, 1},
		 { 2, 1, 0,-1},
		 { 2,-1,-1, 1},
		 { 2,-1, 0, 1},
		 { 2,-1,-1,-1},
		 { 0, 1,-1,-1},
		 { 4, 0,-1,-1},
		 { 0, 1, 0, 1},
		 { 0, 0, 0, 3},
		 { 0, 1,-1, 1},
		 { 1, 0, 0, 1},
		 { 0, 1, 1, 1},
		 { 0, 1, 1,-1},
		 { 0, 1, 0,-1},
		 { 1, 0, 0,-1},
		 { 0, 0, 3, 1},
		 { 4, 0, 0,-1},
		 { 4, 0,-1, 1},
		 { 0, 0, 1,-3},
		 { 4, 0,-2, 1},
		 { 2, 0, 0,-3},
		 { 2, 0, 2,-1},
		 { 2,-1, 1,-1},
		 { 2, 0,-2, 1},
		 { 0, 0, 3,-1},
		 { 2, 0, 2, 1},
		 { 2, 0,-3,-1},
		 { 2, 1,-1, 1},
		 { 2, 1, 0, 1},
		 { 4, 0, 0, 1},
		 { 2,-1, 1, 1},
		 { 2,-2, 0,-1},
		 { 0, 0, 1, 3},
		 { 2, 1, 1,-1},
		 { 1, 1, 0,-1},
		 { 1, 1, 0, 1},
		 { 0, 1,-2,-1},
		 { 2, 1,-1,-1},
		 { 1, 0, 1, 1},
		 { 2,-1,-2,-1},
		 { 0, 1, 2, 1},
		 { 4, 0,-2,-1},
		 { 4,-1,-1,-1},
		 { 1, 0, 1,-1},
		 { 4, 0, 1,-1},
		 { 1, 0,-1,-1},
		 { 4,-1, 0,-1},
		 { 2,-2, 0, 1}
		};
	  const int sigma_r[60] = {
	   -20905355,
		-3699111,
		-2955968,
		 -569925,
		   48888,
		   -3149,
		  246158,
		 -152138,
		 -170733,
		 -204586,
		 -129620,
		  108743,
		  104755,
		   10321,
			   0,
		   79661,
		  -34782,
		  -23210,
		  -21636,
		   24208,
		   30824,
		   -8379,
		  -16675,
		  -12831,
		  -10445,
		  -11650,
		   14403,
		   -7003,
			   0,
		   10056,
			6322,
		   -9884,
			5751,
			   0,
		   -4950,
			4130,
			   0,
		   -3958,
			   0,
			3258,
			2616,
		   -1897,
		   -2117,
			2354,
			   0,
			   0,
		   -1423,
		   -1117,
		   -1571,
		   -1739,
			   0,
		   -4421,
			   0,
			   0,
			   0,
			   0,
			1165,
			   0,
			   0,
			8752
				  };
	  const int sigma_l[60] = {
		6288774,
		1274027,
		 658314,
		 213618,
		-185116,
		-114332,
		  58793,
		  57066,
		  53322,
		  45758,
		 -40923,
		 -34720,
		 -30383,
		  15327,
		 -12528,
		  10980,
		  10675,
		  10034,
		   8548,
		  -7888,
		  -6766,
		  -5163,
		   4987,
		   4036,
		   3994,
		   3861,
		   3665,
		  -2689,
		  -2602,
		   2390,
		  -2348,
		   2236,
		  -2120,
		  -2069,
		   2048,
		  -1773,
		  -1595,
		   1215,
		  -1110,
		   -892,
		   -810,
			759,
		   -713,
		   -700,
			691,
			596,
			549,
			537,
			520,
		   -487,
		   -399,
		   -381,
			351,
		   -340,
			330,
			327,
		   -323,
			299,
			294,
			  0
		};
	  const int sigma_b[60] = {
		5128122,
		 280602,
		 277693,
		 173237,
		  55413,
		  46271,
		  32573,
		  17198,
		   9266,
		   8822,
		   8216,
		   4324,
		   4200,
		  -3359,
		   2463,
		   2211,
		   2065,
		  -1870,
		   1828,
		  -1794,
		  -1749,
		  -1565,
		  -1491,
		  -1475,
		  -1410,
		  -1344,
		  -1335,
		   1107,
		   1021,
			833,
			777,
			671,
			607,
			596,
			491,
		   -451,
			439,
			422,
			421,
		   -366,
		   -351,
			331,
			315,
			302,
		   -283,
		   -229,
			223,
			223,
		   -220,
		   -220,
		   -185,
			181,
		   -177,
			176,
			166,
		   -164,
			132,
		   -119,
			115,
			107
		};

	double t,d,m,ms,f,e,ls;// : extended;
	double sr,sl,sb,temp;// : extended;
	double a1,a2,a3; // : extended;
	double lambda,beta,delta; //: extended;
	int i; //: integer;


	t = (jdate - 2451545.0)/36525.0;

	//(* mean elongation of the moon  
	d = 297.8502042+(445267.1115168+(-0.0016300+(1.0/545868-1.0/113065000*t)*t)*t)*t;

	//(* mean anomaly of the sun  
	m =357.5291092+(35999.0502909+(-0.0001536+1.0/24490000*t)*t)*t;

	//(* mean anomaly of the moon  
	ms =134.9634114+(477198.8676313+(0.0089970+(1.0/69699-1.0/1471200*t)*t)*t)*t;

	//(* argument of the longitude of the moon  
	f =93.2720993+(483202.0175273+(-0.0034029+(-1.0/3526000+1.0/863310000*t)*t)*t)*t;

	//(* correction term due to excentricity of the earth orbit  
	e = 1.0+(-0.002516-0.0000074*t)*t;

	//(* mean longitude of the moon  
	ls =218.3164591+(481267.88134236+(-0.0013268+(1.0/538841-1.0/65194000*t)*t)*t)*t;

	//(* arguments of correction terms  
	a1 =119.75+131.849*t;
	a2 =53.09+479264.290*t;
	a3 =313.45+481266.484*t;

	sr =0;
	for(i = 0; i < 60; i++)
	{
		temp =sigma_r[i]*cos_d( arg_lr[i][0]*d
							   +arg_lr[i][1]*m
							   +arg_lr[i][2]*ms
							   +arg_lr[i][3]*f);
		if (abs(arg_lr[i][1])==1) temp =temp*e;
		if (abs(arg_lr[i][1])==2) temp =temp*e*e;
		sr =sr+temp;
	}

	sl =0;
	for(i =0; i < 60; i++)
	{
		temp =sigma_l[i]*sin_d( arg_lr[i][0]*d
							   +arg_lr[i][1]*m
							   +arg_lr[i][2]*ms
							   +arg_lr[i][3]*f);
		if (abs(arg_lr[i][1])==1) temp =temp*e;
		if (abs(arg_lr[i][1])==2) temp =temp*e*e;
		sl =sl+temp;
	}

	//(* correction terms  
	sl =sl +3958*sin_d(a1)
		+1962*sin_d(ls-f)
		+318*sin_d(a2);
	sb =0;
	for(i=0; i < 60; i++)
	{
		temp = sigma_b[i]*sin_d( arg_b[i][0]*d
							   +arg_b[i][1]*m
							   +arg_b[i][2]*ms
							   +arg_b[i][3]*f);
		if (abs(arg_b[i][1])==1) temp=temp*e;
		if (abs(arg_b[i][1])==2) temp=temp*e*e;
		sb=sb+temp;
	}

	//(* correction terms  
	sb =sb -2235*sin_d(ls)
		  +382*sin_d(a3)
		  +175*sin_d(a1-f)
		  +175*sin_d(a1+f)
		  +127*sin_d(ls-ms)
		  -115*sin_d(ls+ms);

	lambda=ls+sl/1000000;
	beta=sb/1000000;
	delta=385000.56+sr/1000;

	moon.radius = delta;
	moon.longitude_deg =lambda;
	moon.latitude_deg = beta;

	calc_geocentric(moon.longitude_deg, moon.latitude_deg, moon.rektaszension, moon.declination, jdate);

}

/*
#define _rise    0
#define _set     1
#define _transit 2


//  function interpolation(y1,y2,y3,n: extended):extended;  
double interpolation(double y1, double y2, double y3, double n)
{
	double a, b, c;

	a = y2-y1;
	b = y3-y2;
	if (a>100)
		a -= 360;
	if (a<-100)
		a += 360;
	if (b>100)
		b -= 360;
	if (b<-100)
		b += 360;
	c = b - a;
	return y2+0.5*n*(a+b+n*c);
}

double correction(double m, int kind, MOONDATA pos1, MOONDATA pos2, MOONDATA pos3, double longitude, double latitude, double theta0, double h0)
{
	double alpha,delta,h, height;

	alpha = interpolation(pos1.rektaszension,
                       pos2.rektaszension,
                       pos3.rektaszension,
                       m);
	delta = interpolation(pos1.declination,
                       pos2.declination,
                       pos3.declination,
                       m);
	h = put_in_360((theta0+360.985647*m)-longitude-alpha);
	if (h>180) 
		h = h-360;

	height = arcsin_d(sin_d(latitude)*sin_d(delta)
                   +cos_d(latitude)*cos_d(delta)*cos_d(h));

	switch(kind)
	{
		case 0:   return (-h/360);
		case 1: case 2: return ((height-h0)/(360.0*cos_d(delta)*cos_d(latitude)*sin_d(h)));
		default: return 0;
	}
}*/

// star_time(JULIANDATE h)
// correct_position
// double frac(double)

double star_time(JULIANDATE date)
{
	double jd, t;
	double delta_phi, epsilon;

	jd = date;
	t =(jd-2451545.0)/36525.0;
	calc_epsilon_phi(date,delta_phi,epsilon);
	return put_in_360(280.46061837+360.98564736629*(jd-2451545.0)+
                     t*t*(0.000387933-t/38710000)+
                     delta_phi*cos_d(epsilon) );
}

double MoonDistance(JULIANDATE jdate)
{
	const int arg_lr[60][4] = {
		 { 0, 0, 1, 0},		 { 2, 0,-1, 0},		 { 2, 0, 0, 0},		 { 0, 0, 2, 0},		 { 0, 1, 0, 0},
		 { 0, 0, 0, 2},		 { 2, 0,-2, 0},		 { 2,-1,-1, 0},		 { 2, 0, 1, 0},		 { 2,-1, 0, 0},
		 { 0, 1,-1, 0},		 { 1, 0, 0, 0},		 { 0, 1, 1, 0},		 { 2, 0, 0,-2},		 { 0, 0, 1, 2},
		 { 0, 0, 1,-2},		 { 4, 0,-1, 0},		 { 0, 0, 3, 0},		 { 4, 0,-2, 0},		 { 2, 1,-1, 0},
		 { 2, 1, 0, 0},		 { 1, 0,-1, 0},		 { 1, 1, 0, 0},		 { 2,-1, 1, 0},		 { 2, 0, 2, 0},
		 { 4, 0, 0, 0},		 { 2, 0,-3, 0},		 { 0, 1,-2, 0},		 { 2, 0,-1, 2},		 { 2,-1,-2, 0},
		 { 1, 0, 1, 0},		 { 2,-2, 0, 0},		 { 0, 1, 2, 0},		 { 0, 2, 0, 0},		 { 2,-2,-1, 0},
		 { 2, 0, 1,-2},		 { 2, 0, 0, 2},		 { 4,-1,-1, 0},		 { 0, 0, 2, 2},		 { 3, 0,-1, 0},
		 { 2, 1, 1, 0},		 { 4,-1,-2, 0},		 { 0, 2,-1, 0},		 { 2, 2,-1, 0},		 { 2, 1,-2, 0},
		 { 2,-1, 0,-2},		 { 4, 0, 1, 0},		 { 0, 0, 4, 0},		 { 4,-1, 0, 0},		 { 1, 0,-2, 0},
		 { 2, 1, 0,-2},		 { 0, 0, 2,-2},		 { 1, 1, 1, 0},		 { 3, 0,-2, 0},		 { 4, 0,-3, 0},
		 { 2,-1, 2, 0},		 { 0, 2, 1, 0},		 { 1, 1,-1, 0},		 { 2, 0, 3, 0},		 { 2, 0,-1,-2}
	   };

	  const int sigma_r[60] = {
	   -20905355,		-3699111,		-2955968,		 -569925,		   48888,		   -3149,		  246158,
		 -152138,		 -170733,		 -204586,		 -129620,		  108743,		  104755,		   10321,
			   0,		   79661,		  -34782,		  -23210,		  -21636,		   24208,		   30824,
		   -8379,		  -16675,		  -12831,		  -10445,		  -11650,		   14403,		   -7003,
			   0,		   10056,			6322,		   -9884,			5751,			   0,		   -4950,
			4130,			   0,		   -3958,			   0,			3258,			2616,		   -1897,
		   -2117,			2354,			   0,			   0,		   -1423,		   -1117,		   -1571,
		   -1739,			   0,		   -4421,			   0,			   0,			   0,			   0,
			1165,			   0,			   0,			8752
				  };

	double temp, t,d,m,ms,f,e,ls,sr;// : extended;

	t = (jdate - 2451545.0)/36525.0;

	//(* mean elongation of the moon  
	d = 297.8502042+(445267.1115168+(-0.0016300+(1.0/545868-1.0/113065000*t)*t)*t)*t;

	//(* mean anomaly of the sun  
	m =357.5291092+(35999.0502909+(-0.0001536+1.0/24490000*t)*t)*t;

	//(* mean anomaly of the moon  
	ms =134.9634114+(477198.8676313+(0.0089970+(1.0/69699-1.0/1471200*t)*t)*t)*t;

	//(* argument of the longitude of the moon  
	f =93.2720993+(483202.0175273+(-0.0034029+(-1.0/3526000+1.0/863310000*t)*t)*t)*t;

	//(* correction term due to excentricity of the earth orbit  
	e = 1.0+(-0.002516-0.0000074*t)*t;

	//(* mean longitude of the moon  
	ls =218.3164591+(481267.88134236+(-0.0013268+(1.0/538841-1.0/65194000*t)*t)*t)*t;
	
	sr =0;

	int i;
	for(i = 0; i < 60; i++)
	{
		temp =sigma_r[i]*cos_d( arg_lr[i][0]*d
							   +arg_lr[i][1]*m
							   +arg_lr[i][2]*ms
							   +arg_lr[i][3]*f);
		if (abs(arg_lr[i][1])==1) temp =temp*e;
		if (abs(arg_lr[i][1])==2) temp =temp*e*e;
		sr =sr+temp;
	}
	
	
	return 385000.56+sr/1000;

}

#define AU 149597869.0
double GetAyanamsa(JULIANDATE);
//

void correct_parallax(MOONDATA & position, JULIANDATE jdate, double latitude, double longitude)
{
	double moondist = MoonDistance(jdate);
	double longit = position.longitude_deg + longitude;
	double newval = cos_d(longit) * arctan2_d(6378.14, moondist);

	position.longitude_deg = put_in_360(position.longitude_deg + newval);
}

void correct_position(MOONDATA & position, JULIANDATE jdate, double latitude, double longitude, double height)
{
	double u,h,delta_alpha;
	double rho_sin, rho_cos;
	const double b_a=0.99664719;

	u =arctan_d(b_a*b_a*tan_d(latitude));
	rho_sin =b_a*sin_d(u)+height/6378140.0*sin_d(latitude);
	rho_cos =cos_d(u)+height/6378140.0*cos_d(latitude);

	position.parallax = arcsin_d(sin_d(8.794/3600)/(MoonDistance(jdate) / AU));

	h = star_time(jdate)-longitude-position.rektaszension;
	delta_alpha = arctan_d(
                (-rho_cos*sin_d(position.parallax)*sin_d(h))/
                (cos_d(position.declination)-
                  rho_cos*sin_d(position.parallax)*cos_d(h)));
	position.rektaszension =position.rektaszension+delta_alpha;
	position.declination =arctan_d(
      (( sin_d(position.declination)
        -rho_sin*sin_d(position.parallax))*cos_d(delta_alpha))/
      ( cos_d(position.declination)
       -rho_cos*sin_d(position.parallax)*cos_d(h)));
}

double frac(double x) 
{
	return x - floor(x);
}

void calc_horizontal(MOONDATA &coord, JULIANDATE date, double longitude, double latitude)
{
	double h;

	h = put_in_360(star_time(date)-coord.rektaszension+longitude);
	
	coord.azimuth = rad2deg( atan2(sin_d(h),
                           cos_d(h)*sin_d(latitude)-
                           tan_d(coord.declination)*cos_d(latitude) ));
	
	coord.elevation = rad2deg(asin(sin_d(latitude)*sin_d(coord.declination)+
                            cos_d(latitude)*cos_d(coord.declination)*cos_d(h)));
}

//==================================================================================
//
//==================================================================================

double MoonCalcElevation(EARTHDATA e, VCTIME vc)
{
	static MOONDATA moon;
	double d = vc.GetJulianComplete();
	MoonCalc(d, moon, e);
	correct_position(moon, d, e.latitude_deg, e.longitude_deg, 0);
	calc_horizontal(moon, d, e.longitude_deg, e.latitude_deg);

	return moon.elevation;
}

//==================================================================================
//
//==================================================================================

void CalcMoonTimes(EARTHDATA e, VCTIME vc, double nDaylightSavingShift, DAYTIME &rise, DAYTIME &set)
{
	double UT;
	int i;
	double prev_elev;
	int nType, nFound = 0;
	double a, ae, b, be, c, ce, elev;

	rise.SetValue(-1);
	set.SetValue(-1);

	// inicializacia prvej hodnoty ELEVATION
	vc.shour = (-nDaylightSavingShift - 1.0) / 24.0;
	prev_elev = MoonCalcElevation(e, vc);

	// prechod cez vsetky hodiny
	for(UT = (- 0.1 - nDaylightSavingShift); UT <= (24.1 - nDaylightSavingShift); UT+=1.0)
	{
		vc.shour = UT / 24.0;
		elev = MoonCalcElevation(e, vc);
		
		if (prev_elev * elev <= 0.0)
		{
			if (prev_elev <= 0.0)
			{
				nType = 0x1;
			}
			else
			{
				nType = 0x2;
			}
			
			a = UT - 1.0;
			ae = prev_elev;
			b = UT;
			be = elev;
			for(i = 0; i < 20; i++)
			{
				c = (a + b) / 2.0;
				vc.shour = c / 24.0;
				ce = MoonCalcElevation(e, vc);
				if (ae*ce <= 0.0)
				{
					be = ce;
					b = c;
				}
				else
				{
					ae = ce;
					a = c;
				}
			}

			if (nType == 1)
			{
				rise.SetDayTime((c + nDaylightSavingShift)/24.0);
			}
			else
			{
				set.SetDayTime((c + nDaylightSavingShift)/24.0);
			}

			nFound |= nType;

			if (nFound == 0x3)
				break;
		}

		prev_elev = elev;
	}

}



