#ifndef _XMESHLIB_CORE_POINT_H_
#define _XMESHLIB_CORE_POINT_H_

#include <assert.h>
#include <math.h>

namespace XMeshLib{
	class Point
	{
	public:
		Point( double x, double y, double z ){ v[0] = x; v[1] = y; v[2] = z;}
		Point() { v[0] = v[1] = v[2] = 0; }
		~Point(){}

		double & operator[]( int i)		  { assert( 0<=i && i<3 ); return v[i]; };
		double   operator()( int i) const { assert( 0<=i && i<3 ); return v[i]; };
		double   operator[]( int i) const { assert( 0<=i && i<3 ); return v[i]; };
		double norm() const { return sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); };
		double norm2() const { return ( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] ); };
		void SetZero(){v[0] = v[1] = v[2] = 0;}

		Point  & operator += ( const Point & p) { v[0] += p(0); v[1] += p(1); v[2] += p(2); return *this; }; 
		Point  & operator -= ( const Point & p) { v[0] -= p(0); v[1] -= p(1); v[2] -= p(2); return *this; };
		Point  & operator *= ( const double  s) { v[0] *= s   ; v[1] *=    s; v[2] *=    s; return *this; };
		Point  & operator /= ( const double  s) { v[0] /= s   ; v[1] /=    s; v[2] /=    s; return *this; };

		double   operator*( const Point & p ) const 
		{
			return v[0] * p[0] + v[1] * p[1] + v[2] * p[2]; 
		};

		Point   operator+( const Point & p  ) const
		{
			Point r( v[0] + p[0], v[1] + p[1], v[2] + p[2] );
			return r;
		};
		Point   operator-( const Point & p  ) const
		{
			Point r( v[0] - p[0], v[1] - p[1], v[2] - p[2] );
			return r;
		};
		Point   operator*( const double s  ) const
		{
			Point r( v[0] * s, v[1] * s, v[2] * s );
			return r;
		};
		Point   operator/( const double s  ) const
		{
			Point r( v[0] / s, v[1] / s, v[2] / s );
			return r;
		};

		Point operator^( const Point & p2) const
		{
			Point r( v[1] * p2[2] - v[2] * p2[1],
					 v[2] * p2[0] - v[0] * p2[2],
					 v[0] * p2[1] - v[1] * p2[0]);
			return r;
		};

		Point operator-() const
		{
			Point p(-v[0],-v[1],-v[2]);
			return p;
		};
	
		double v[3];
	};

}//name space XMeshLib

#endif //_XMESHLIB_CORE_POINT_H_ defined