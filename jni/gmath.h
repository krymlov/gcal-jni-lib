#ifndef _VCAL_G_MATH_FUNCTUIONS
#define _VCAL_G_MATH_FUNCTUIONS

// PORTABLE

// pi
#define pi 3.1415926535897932385
// 2 * pi
#define pi2 6.2831853071795864769
// pi / 180
#define rads 0.0174532925199432958


double cos_d(double x);
double sin_d(double x);
double tan_d(double x);
double arcsin_d(double x);
double arctan_d(double x);
double arccos_d(double x);

double arctan2_d(double x, double y);
double put_in_360(double id);
double put_in_180(double in_d);
double norm_val(double v);
int sgn(double d);
double rad2deg(double x);
double deg2rad(double x);



#endif