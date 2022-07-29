#include "math.h"
#include "gmath.h"
// PORTABLE 

/*********************************************************************************/
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/

/////////////////////////////////////
// input value: arc in degrees

double cos_d(double x)
{
	return cos( x * rads);
}

/////////////////////////////////////
// input value: arc in degrees

double sin_d(double x)
{
	return sin( x * rads);
}

/////////////////////////////////////
// input value: arc in degrees
// it is calculating arctan(x/y)
// with testing values

double arctan2_d(double x, double y)
{
	return atan2(x, y) / rads;
}

/////////////////////////////////////
// input value: arc in degrees
// output value: tangens 

double tan_d(double x)
{
	return tan(x * rads);
}

/////////////////////////////////////
// input value: -1.0 .. 1.0
// output value: -180 deg .. 180 deg

double arcsin_d(double x)
{
	return asin(x) / rads;
}

double arccos_d(double x)
{
	return acos(x) / rads;
}

double arctan_d(double x)
{
	return atan(x) / rads;
}

// modulo 1

double norm_val(double v)
{
	double v2 = v - floor(v);
	while (v2 < 0.0)
		v2 += 1.0;
	while (v2 > 1.0)
		v2 -= 1.0;
	return v2;
}

// modulo 360

double put_in_360(double id)
{
	//double d = id;
	double d = fmod(id, 360.0);
	while(d >= 360.0) d -= 360.0;
	while(d < 0.0) d += 360.0;
	return d;
}

// modulo 360 but in range -180deg .. 180deg
// used for comparision values around 0deg
// so difference between 359deg and 1 deg 
// is not 358 degrees, but 2 degrees (because 359deg = -1 deg)

double put_in_180(double in_d)
{
	double d = in_d;

	while(d < -180.0)
	{
		d += 360.0;
	}
	while(d > 180.0)
	{
		d -= 360.0;
	}

	return d;
}

// sign of the number
// -1: number is less than zero
// 0: number is zero
// +1: number is greater than zero

int sgn(double d)
{
	if (d > 0.0)
		return 1;
	if (d < 0.0)
		return -1;
	return 0;
}

double deg2rad(double x)
{
	return x * rads;
}

double rad2deg(double x)
{
	return x / rads;
}
