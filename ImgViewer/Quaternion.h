 #ifndef __QUATERNION_H
#define __QUATERNION_H
#include <cmath>
#include <cassert>
#include <string>
#include <iostream>
#include "Vector3.h"


template <class _T> class Quat;


template <class _T>
inline std::istream& operator >> (std::istream &is, Quat<_T> &q) { is >> q.x >> q.y >> q.z >> q.w; return is; }

template <class _T>
inline std::ostream& operator << (std::ostream &os, const Quat <_T> &q) { os << q.x << ' ' << q.y << ' ' << q.z << ' ' << q.w; return os; }


template <class _T>
class Quat 
{ 
public:
	Quat() { }
	Quat(_T xc, _T yc, _T zc, _T wc) : x(xc), y(yc), z(zc), w(wc) {}
	Quat(const Quat& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}

	// No error hannling for c-style array acess by subscript
	_T& operator [] (int i) 
	{
		assert(i >= 0 && i < 4);
		if (i == 0) return x;
		else if (i == 1) return y;
		else if (i == 2) return z;
		else return w;
	}

	bool operator == (const Quat &q)
	{ return (x == q.x && y == q.y && z == q.z && w == q.w); }

	bool operator != (const Quat &q) { return !((*this) == q); } 

	Quat& operator = (const Quat &q) 
	{ x = q.x; y = q.y; z = q.z;  w = q.w; return *this; } 

	Quat operator - () const { return Quat(-x,-y,-z,-w); }

	Quat operator + (const Quat &q) const 
	{ return Quat(x + q.x,y + q.y,z + q.z,w + q.w); } 

	Quat operator - (const Quat &q) const 
	{ return Quat(x - q.x, y - q.y, z - q.z, w - q.w); } 

	Quat operator * (const _T &s) const { return Quat(s*x,s*y,s*z,s*w); } 
	
	//To combine rotations, use the product qSecond*qFirst,
	//which gives the effect of rotating by qFirst then qSecond
	Quat operator * (const Quat &q) const
	{
		_T i, j, k, r;

		i = w*q.x + x*q.w + y*q.z - z*q.y;
		j = w*q.y + y*q.w + z*q.x - x*q.z;
		k = w*q.z + z*q.w + x*q.y - y*q.x;
		r = w*q.w - x*q.x - y*q.y - z*q.z;

		return Quat(i, j, k, r);
	}


	Quat& operator += (const Quat &q) { x += q.x; y += q.y; z += q.z; w += q.w; return *this; }
	Quat& operator -= (const Quat &q) { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return *this; }
	Quat& operator *= (const _T &s) { x *= s; y *= s; z *= s; w *= s; return *this; }
	Quat& operator /= (const _T &s) { x /= s; y /= s; z /= s; w /= s; return *this; }
	Quat& operator *= (const Quat &q) 
	{
		_T i, j, k, r;

		i = w*q.x + x*q.w + y*q.z - z*q.y;
		j = w*q.y + y*q.w + z*q.x - x*q.z;
		k = w*q.z + z*q.w + x*q.y - y*q.x;
		r = w*q.w - x*q.x - y*q.y - z*q.z;

		x = i; y = j; z = k; w = r;
		return *this;
	}


	Quat conjugate() const { return Quat(-x,-y,-z,w); }

	Quat inverse() const 
	{
		_T n = norm();
		if (n == (_T)0) n = (_T)1;
		return (conjugate() / n);
	}

	_T norm2() const { return (_T)(x*x + y*y + z*z + w*w); }
	_T norm() const { return (_T)sqrt(x*x + y*y + z*z + w*w); }
	_T norm2(const Quat &q) { return (_T)(q.x*q.x + q.y*q.y + q.z*q.z); }
	_T norm(const Quat &q) { return (_T)sqrt(q.x*q.x + q.y*q.y + q.z*q.z);}



	void normalize()
	{
		_T n = norm();
		if (n == (_T)0.0) 
		{
			w = (_T)1.0;
			x = y = z = (_T)0.0;
			return;
		}
		x /= n;
		y /= n; 
		z /= n; 
		w /= n;
	}

	void FromAxisAngle(const Vector3<_T> &axis, float angle);
	void ToAxisAngle(Vector3<_T> &axis, float &angle);

	void RotateVec(Vector3<_T> &v) const;

public:
	_T x,y,z,w;
};

////////////////////////////////////////////////////////////////////////////////////////

// Create a unit quaternion representing the rotation of the 3D vector v by
// an angle 2*theta about the 3D axis u. The quaternion is q = cos(theta)+u*sin(theta)
template <class _T>
void Quat<_T>::FromAxisAngle(const Vector3<_T> &axis, float angle)
{
	float c2 = cos(0.5f*angle);
	float s2 = sin(0.5f*angle);

	Vector3<_T> naxis = axis;
	naxis.normalize();

	w = c2;
	x = naxis.x * (_T)s2;
	y = naxis.y * (_T)s2;
	z = naxis.z * (_T)s2;
	return;
}

// Extract the rotation and the axis of rotation in a quaternion representation
template <class _T>
void Quat<_T>::ToAxisAngle(Vector3<_T> &axis, float &angle)
{
	angle = 2.0f * acos(w);

	if (angle == 0.0f) 
	{
		axis.x = (_T)0;
		axis.y = (_T)0;
		axis.z = (_T)1;
		return;
	}

	axis.x = x;
	axis.y = y;
	axis.z = z;
	axis.normalize();
	return;
}


template <class _T>
void Quat<_T>::RotateVec(Vector3<_T> &v) const
{
	float vlen = v.norm();
	if (vlen == 0.0f)
		return;

	Quat<_T> vq(v.x, v.y, v.z, (_T)0);
	Quat<_T> conj = this->conjugate();
	Quat<_T> temp = (*this)*vq;
	temp = temp*conj;

	v.x = temp.x; 
	v.y = temp.y; 
	v.z = temp.z;
	
	v.normalize();
	v *= vlen;
	return;
}



typedef Quat<float>	Quatf;
typedef Quat<double> Quatd;


#endif  // __QUATERNION_H 

