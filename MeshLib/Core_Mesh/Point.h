#ifndef _POINT_H_
#define _POINT_H_
#include <cmath>

class Point
{
public:
	Point( double x, double y, double z ){ v[0] = x; v[1] = y; v[2] = z;}
	Point() { v[0] = v[1] = v[2] = 0; }
	~Point() {;}

	double & operator[](int i) {return v[i];}
	double norm() const { return sqrt( fabs( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] ) );}
	double norm2() const { return fabs( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] ) ;}

	Point  & operator += ( Point & p) { v[0] += p[0]; v[1] += p[1]; v[2] += p[2]; return *this; } 
	Point  & operator -= ( Point & p) { v[0] -= p[0]; v[1] -= p[1]; v[2] -= p[2]; return *this; }
	Point  & operator *= ( double  s) { v[0] *= s   ; v[1] *=    s; v[2] *=    s; return *this; }
	Point  & operator /= ( double  s) { v[0] /= s   ; v[1] /=    s; v[2] /=    s; return *this; }

	double   operator*( Point & p ) const 
	{
		return v[0] * p[0] + v[1] * p[1] + v[2] * p[2]; 
	}
	Point   operator+( Point & p  ) const
	{
		Point r( v[0] + p[0], v[1] + p[1], v[2] + p[2] );
		return r;
	}
	Point   operator-( Point & p  ) const
	{
		Point r( v[0] - p[0], v[1] - p[1], v[2] - p[2] );
		return r;
	}
	Point   operator*( double s  ) const
	{
		Point r( v[0] * s, v[1] * s, v[2] * s );
		return r;
	}
	Point   operator/( double s  ) const
	{
		Point r( v[0] / s, v[1] / s, v[2] / s );
		return r;
	}

	Point operator^( Point & p2) const
	{
		Point r( v[1] * p2[2] - v[2] * p2[1],
				 v[2] * p2[0] - v[0] * p2[2],
				 v[0] * p2[1] - v[1] * p2[0]);
		return r;
	}

	Point operator-() const
	{
		Point p(-v[0],-v[1],-v[2]);
		return p;
	}

public:
	double v[3];
};

#endif