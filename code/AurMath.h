#ifndef GAMATH_H
#define GAMATH_H

#include <cmath>
#include <memory.h>
#include <string>

#pragma warning( disable : 4201 )

namespace Aurora
{
	template<typename T> class Vector2;
	template<typename T> class Vector3;
	template<typename T> class Vector4;
	template<typename T> class Matrix3;
	template<typename T> class Matrix4;
	template<typename T> class Quaternion;

//-----------------------------------------------------------
template<typename real>
class Math
{
public:
	static inline real Abs(real value);

	static inline real Sin(real angle);
	static inline real Cos(real angle);
	static inline real Tan(real angle);

	static inline real ASin(real angle);
	static inline real ACos(real angle);
	static inline real ATan(real angle);
	static inline real ATan2(real x, real y);

	static inline real Sqrt(real value);
	static inline real Pow(real x, real y);

	static inline real Floor(real x);
	static inline real Ceil(real x);
	static inline real Mod(real x, real y);
	static inline real Exp(real value);
	static inline real Log(real value);

	static inline real Min(real val1, real val2);
	static inline real Max(real val1, real val2);
	static inline real Clamp(real val, real min, real max);
	static inline real GetLerp(real min, real max, real middle);

	static inline bool IsZero(real value);

    static const real ZERO_TOLERANCE;
    static const real MAX_VALUE;
	
	static const real PI;
	static const real HALF_PI;

    static const real DEG_TO_RAD;
    static const real RAD_TO_DEG;

};
//-----------------------------------------------------------

//--------------------------------------------------------
template<>
inline float Math<float>::Abs(float value)
{
	return fabsf(value);
}
//--------------------------------------------------------
template<>
inline float Math<float>::Sin(float angle)
{
	return sinf(angle);
}
//--------------------------------------------------------
template<>
inline float Math<float>::Cos(float angle)
{
	return cosf(angle);
}
//--------------------------------------------------------
template<>
inline float Math<float>::Tan(float angle)
{
	return tanf(angle);
}
//--------------------------------------------------------
template<>
inline float Math<float>::ASin(float angle)
{
	return asinf(angle);
}
//--------------------------------------------------------
template<>
inline float Math<float>::ACos(float angle)
{
	return acosf(angle);
}
//--------------------------------------------------------
template<>
inline float Math<float>::ATan(float angle)
{
	return atanf(angle);
}
//--------------------------------------------------------
template<>
inline float Math<float>::ATan2(float x, float y)
{
	return atan2f(x, y);
}
//--------------------------------------------------------
template<>
inline float Math<float>::Sqrt(float value)
{
	return sqrtf(value);
}

//--------------------------------------------------------
template<>
inline float Math<float>::Pow(float x, float y)
{
	return powf(x, y);
}
//--------------------------------------------------------
template<>
inline float Math<float>::Floor(float x)
{
	return floorf(x);
}
//--------------------------------------------------------
template<>
inline float Math<float>::Ceil(float x)
{
	return ceilf(x);
}
//--------------------------------------------------------
template<>
inline float Math<float>::Mod(float x, float y)
{
	return fmodf(x, y);
}
//--------------------------------------------------------
template<>
inline float Math<float>::Exp(float value)
{
	return expf(value);
}
//--------------------------------------------------------
template<>
inline float Math<float>::Log(float value)
{
	return logf(value);
}
//--------------------------------------------------------
template<>
inline float Math<float>::Min(float val1, float val2)
{
	return val1 < val2 ? val1 : val2;
}
//--------------------------------------------------------
template<>
inline float Math<float>::Max(float val1, float val2)
{
	return val1 > val2 ? val1 : val2;
}
//--------------------------------------------------------
template<>
inline float Math<float>::Clamp(float val, float min, float max)
{
	return Math<float>::Min(Math<float>::Max(val, min), max);
}
//--------------------------------------------------------
template<>
inline float Math<float>::GetLerp(float min, float max, float middle)
{
	return (middle - min) / (max - min);
}
//--------------------------------------------------------
template<>
inline bool Math<float>::IsZero(float value)
{
	return (value >= -Math<float>::ZERO_TOLERANCE) && (value <= Math<float>::ZERO_TOLERANCE);
}

typedef Math<float> Mathf;


//////////////////////////////////////////////////////////////////////////


template<typename T>
class Vector2
{
public:
	Vector2();
	Vector2(T fx, T fy);
	Vector2(const Vector2<T>& v);
	Vector2(const T v[2]);


	inline const Vector2<T>& operator = (const Vector2<T>& v);

	inline void Set(T fx, T fy);
	inline void Set(const T* ptr);

	inline T& operator [](size_t index);
	inline const T& operator[](size_t index) const;

	inline Vector2<T> operator -() const;
	inline Vector2<T> operator +(const Vector2<T>& v) const;
	inline Vector2<T> operator -(const Vector2<T>& v) const;
	inline Vector2<T> operator *(T scale) const;
	inline Vector2<T> operator /(T scale) const;

	inline const Vector2<T>& operator +=(const Vector2<T>& v);
	inline const Vector2<T>& operator -=(const Vector2<T>& v);
	inline const Vector2<T>& operator *=(T scale);
	inline const Vector2<T>& operator /=(T scale);

	inline T*		Ptr();
	inline const T* Ptr() const;

	inline T LengthSQ()	const;
	inline T Length()	const;

	inline void Normalize();

	T Dot(const Vector2<T>& rhs) const;
	static T Dot(const Vector2<T>& lhs, const Vector2<T>& rhs);

	static Vector2<T> Lerp(const Vector2<T>& a, const Vector2<T>& b, T s);

	static const Vector2 ZERO;
	static const Vector2 UNIT_X;
	static const Vector2 UNIT_Y;

	T	x, y;
};


//---------------------------------------------------------------------
template<typename T>
class Vector3
{
public:
	Vector3();
	Vector3(T fx, T fy, T fz);
	Vector3(const Vector3<T>& v);
	Vector3(const T v[3]);

	inline const Vector3<T>& operator = (const Vector3<T>& v);

	inline void Set(T fx, T fy, T fz);
	inline void Set(const T* ptr);

	inline T& operator [](size_t index);
	inline const T& operator[](size_t index) const;

	inline Vector3<T> operator -() const;
	inline Vector3<T> operator +(const Vector3<T>& v) const;
	inline Vector3<T> operator -(const Vector3<T>& v) const;
	inline Vector3<T> operator *(T scale) const;
	inline Vector3<T> operator /(T scale) const;

	inline const Vector3<T>& operator +=(const Vector3<T>& v);
	inline const Vector3<T>& operator -=(const Vector3<T>& v);
	inline const Vector3<T>& operator *=(T scale);
	inline const Vector3<T>& operator /=(T scale);

	inline T LengthSQ()	const;
	inline T Length()	const;

	inline void Normalize();

	inline T Dot(const Vector3<T>& v) const;
	static T Dot(const Vector3<T>& l, const Vector3<T>& r);

	inline Vector3<T> Cross(const Vector3<T>& v) const;
	static Vector3<T> Cross(const Vector3<T>& l, const Vector3<T>& r);

	inline T*		Ptr();
	inline const T* Ptr() const;

	static Vector3<T> Lerp(const Vector3<T>& a, const Vector3<T>& b, T s);

	inline std::string ToString() const;
	static Vector3<T> FromString(const char* str);

	static const Vector3 ZERO;
	static const Vector3 ONE;
	static const Vector3 UNIT_X;
	static const Vector3 UNIT_Y;
	static const Vector3 UNIT_Z;

	T	x, y, z;
};


//---------------------------------------------------------------------
template<typename T>
class Vector4
{
public:
	Vector4();
	Vector4(T fx, T fy, T fz, T fw);
	Vector4(const Vector4<T>& v);

	inline const Vector4<T>& operator = (const Vector4<T>& v);

	inline void Set(T fx, T fy, T fz, T fw);
	inline void Set(const T* ptr);

	inline T& operator [](size_t index);
	inline const T& operator[](size_t index) const;

	inline Vector4<T> operator -() const;
	inline Vector4<T> operator +(const Vector4<T>& v) const;
	inline Vector4<T> operator -(const Vector4<T>& v) const;
	inline Vector4<T> operator *(T scale) const;
	inline Vector4<T> operator /(T scale) const;

	inline const Vector4<T>& operator +=(const Vector4<T>& v);
	inline const Vector4<T>& operator -=(const Vector4<T>& v);
	inline const Vector4<T>& operator *=(T scale);
	inline const Vector4<T>& operator /=(T scale);

	inline Vector3<T> ProjectToVector3() const;

	inline T LengthSQ()	const;
	inline T Length()	const;

	inline void Normalize();

	T Dot(const Vector4<T>& rhs) const;
	static T Dot(const Vector4<T>& lhs, const Vector4<T>& rhs);

	inline std::string ToString() const;
	static Vector3<T> FromString(const char* str);

	inline T*		Ptr();
	inline const T* Ptr() const;

	static const Vector4 ZERO;

	T	x, y, z, w;
};



template<typename real>
class Matrix3
{
public:

	union
	{
		struct
		{
			real        _11, _12, _13;
			real        _21, _22, _23;
			real        _31, _32, _33;
		};
		real m[9];
	};

	Matrix3();
	Matrix3(const Matrix3<real>& matrix);
	Matrix3(const real* pData);
	Matrix3(real m11, real m12, real m13,
		real m21, real m22, real m23,
		real m31, real m32, real m33);

	inline const Matrix3<real>& operator=(const Matrix3<real>& rhl);

	inline void Set(real m11, real m12, real m13,
		real m21, real m22, real m23,
		real m31, real m32, real m33);

	/*	inline const Matrix3<real>& operator +=(const Matrix3<real>& m);
	inline const Matrix3<real>& operator -=(const Matrix3<real>& m);
	inline const Matrix3<real>& operator *=(T scale);
	inline const Matrix3<real>& operator /=(T scale);*/

	Matrix3<real>			operator * (const Matrix3<real>& m);
	Matrix3<real>&			operator *= (const Matrix3<real>& m);

	template<typename U>
	friend Vector3<U>	operator * (const Vector3<U>& v, const Matrix3<U>& m);
	template<typename U>
	friend Vector3<U>&	operator *= (Vector3<U>& v, const Matrix3<U>& m);

	Matrix4<real>		ToMat4() const;

	inline real*		Ptr();
	inline const real*	Ptr() const;
	inline real* GetRow(size_t row) const;

	inline void TransposeSelf();

	// special matrices
	static const Matrix3 ZERO;
	static const Matrix3 IDENTITY;

};



template<typename real>
class Matrix4
{
public:
	// data 
	union
	{
		struct
		{
			real	_11, _12, _13, _14;
			real	_21, _22, _23, _24;
			real	_31, _32, _33, _34;
			real	_41, _42, _43, _44;
		};
		real m[16];
	};

	Matrix4() {}
	Matrix4(const Matrix4<real>& matrix);
	Matrix4(const real* data);
	Matrix4(real m11, real m12, real m13, real m14,
		real m21, real m22, real m23, real m24,
		real m31, real m32, real m33, real m34,
		real m41, real m42, real m43, real m44);

	inline const Matrix4<real>& operator=(const Matrix4<real>& rhl);

	inline void Set(real m11, real m12, real m13, real m14,
		real m21, real m22, real m23, real m24,
		real m31, real m32, real m33, real m34,
		real m41, real m42, real m43, real m44);

	inline Matrix4<real> operator +(const Matrix4<real>& m) const;
	inline Matrix4<real> operator -(const Matrix4<real>& m) const;
	inline Matrix4<real> operator *(real scale) const;
	inline Matrix4<real> operator /(real scale) const;

	inline const Matrix4<real>& operator +=(Matrix4<real>& m);
	inline const Matrix4<real>& operator -=(Matrix4<real>& m);
	inline const Matrix4<real>& operator *=(real scale);
	inline const Matrix4<real>& operator /=(real scale);

	Matrix4<real>			operator * (const Matrix4<real>& m);
	Matrix4<real>&			operator *= (const Matrix4<real>& m);

	template<typename U>
	friend Vector3<U>	operator * (const Vector3<U>& v, const Matrix4<U>& m);

	template<typename U>
	friend Vector3<U>&	operator *= (Vector3<U>& v, const Matrix4<U>& m);
	
	inline real* Ptr();
	inline const real* Ptr() const;
	inline real* GetRow(size_t row) const;
	
	Quaternion<real> ToQuaternion() const;

	inline void TransposeSelf();
	Matrix4<real> Inverse() const;
	const Matrix4<real>& InverseSelf();

	static Matrix4<real> RotateX(float angle); // in rad
	static Matrix4<real> RotateY(float angle);
	static Matrix4<real> RotateZ(float angle);
	static Matrix4<real> Scale(const Vector3<real>& s);
	static Matrix4<real> RotationAxis(const Vector3<real>& axis, real angle);

	static Matrix4<real> Translate(const Vector3<real>& t);
	static Matrix4<real> Transform(const Quaternion<real>& q, const Vector3<real>& t);
	static Matrix4<real> LookAtLH(const Vector3<real>& vEye,
		const Vector3<real>& vAt = Vector3<real>::ZERO,
		const Vector3<real>& vUp = Vector3<real>::UNIT_Z);

	static Matrix4<real> LookAtRH(const Vector3<real>& vEye,
		const Vector3<real>& vAt = Vector3<real>::ZERO,
		const Vector3<real>& vUp = Vector3<real>::UNIT_Z);

	static Matrix4<real> PerspectiveFovLH(real fovy = Math<real>::PI / 4.0f,
											real aspect = 1.0f, real zn = 1.0f, real zf = 1000.0f);

	static Matrix4<real> PerspectiveFovRH(real fovy = Math<real>::PI / 4.0f,
											real aspect = 1.0f, real zn = 1.0f, real zf = 1000.0f);
	
	static Matrix4<real> OrthoLH(real width, real height, real zn = 1.0f, real zf = 1000.0f);
	static Matrix4<real> OrthoRH(real width, real height, real zn = 1.0f, real zf = 1000.0f);
	static Matrix4<real> OrthoOffCenterLH(real l, real r, real b, real t,
											real zn = 1.0f, real zf = 1000.0f);
	static Matrix4<real> OrthoOffCenterRH(real l, real r, real b, real t,
											real zn = 1.0f, real zf = 1000.0f);

	// special matrices
	static const Matrix4 ZERO;
	static const Matrix4 IDENTITY;
};


template<typename T>
class Quaternion
{
public:
	Quaternion(void)	{};
	Quaternion(T fw, T fx, T fy, T fz);
	Quaternion(const Quaternion<T>& q);
	Quaternion(T fx, T fy, T fz);

	inline const Quaternion<T>& operator = (const Quaternion<T>& q);

	inline Quaternion<T> operator -() const;
	inline Quaternion<T> operator +(const Quaternion<T>& q) const;
	inline Quaternion<T> operator -(const Quaternion<T>& q) const;
	inline Quaternion<T> operator *(T scale) const;
	inline Quaternion<T> operator /(T scale) const;

	inline const Quaternion<T>& operator +=(const Quaternion<T>& q);
	inline const Quaternion<T>& operator -=(const Quaternion<T>& q);
	inline const Quaternion<T>& operator *=(T scale);
	inline const Quaternion<T>& operator /=(T scale);

	inline void Set(T fw, T fx, T fy, T fz);
	inline void Set(const T* ptr);
	inline void Set(T fx, T fy, T fz);

	Quaternion<T>		operator * (const Quaternion<T>& r) const;

	template<typename U>
	friend Vector3<U>	operator * (const Vector3<U>& v, const Quaternion<U>& q);
	template<typename U>
	friend Vector3<U>&	operator *= (Vector3<U>& v, const Quaternion<U>& q);

	Quaternion<T> Inverse() const;
	const Quaternion<T>& InverseSelf();

	Matrix3<T>	ToMat3() const;
	Matrix4<T>	ToMat4() const;

	static T Dot(const Quaternion<T>& a, const Quaternion<T>& b);

	static Quaternion<T> FromVectorToVector(const Vector3<T>& vFrom, const Vector3<T>& vTo);
	static Quaternion<T> Slerp(const Quaternion<T>& ql, const Quaternion<T>& qr, T s);
	static Quaternion<T> RotationAxis(const Vector3<T>& v, T angle);

	inline T LengthSQ()	const;
	inline T Length()	const;

	inline void Normalize();

	inline T*		Ptr();
	inline const T* Ptr() const;

	T w, x, y, z;

	static const Quaternion IDENTITY;
};



//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------
template<typename T>
Vector2<T>::Vector2()	{}
//---------------------------------------------------------------
template<typename T>
Vector2<T>::Vector2(T fx, T fy)
{
	x = fx;
	y = fy;
}
//---------------------------------------------------------------
template<typename T>
Vector2<T>::Vector2(const Vector2<T>& v)
{
	x = v.x;
	y = v.y;
}
//---------------------------------------------------------------
template<typename T>
Vector2<T>::Vector2(const T v[2])
{
	x = v[0];
	y = v[1];
}
//---------------------------------------------------------------
template<typename T>
inline const Vector2<T>&
Vector2<T>::operator = (const Vector2<T>& v)
{
	x = v.x;
	y = v.y;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline void
Vector2<T>::Set(T fx, T fy)
{
	x = fx; y = fy;
}
//---------------------------------------------------------------
template<typename T>
inline void
Vector2<T>::Set(const T* ptr)
{
	x = ptr[0];	y = ptr[1];
}
//---------------------------------------------------------------
template<typename T>
inline T& Vector2<T>::operator [](size_t index)
{
	return *(&x + index);
}
//---------------------------------------------------------------
template<typename T>
inline const T& Vector2<T>::operator[](size_t index) const
{
	return *(&x + index);
}
//---------------------------------------------------------------
template<typename T>
inline Vector2<T> Vector2<T>::operator -() const
{
	return Vector2<T>(-x, -y);
}
//---------------------------------------------------------------
template<typename T>
inline Vector2<T>
Vector2<T>::operator +(const Vector2<T>& v) const
{
	return Vector2<T>(x + v.x, y + v.y);
}
//---------------------------------------------------------------
template<typename T>
inline Vector2<T>
Vector2<T>::operator -(const Vector2<T>& v) const
{
	return Vector2<T>(x - v.x, y - v.y);
}
//---------------------------------------------------------------
template<typename T>
inline Vector2<T>
Vector2<T>::operator *(T scale) const
{
	return Vector2<T>(x*scale, y*scale);
}
//---------------------------------------------------------------
template<typename T>
inline Vector2<T>
Vector2<T>::operator /(T scale) const
{
	T reciprocal = (T)1.0 / scale;
	return Vector2<T>(x*reciprocal, y*reciprocal);
}
//---------------------------------------------------------------
template<typename T>
inline const Vector2<T>&
Vector2<T>::operator +=(const Vector2<T>& v)
{
	x += v.x;
	y += v.y;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Vector2<T>&
Vector2<T>::operator -=(const Vector2<T>& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Vector2<T>&
Vector2<T>::operator *=(T scale)
{
	x *= scale;
	y *= scale;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Vector2<T>&
Vector2<T>::operator /=(T scale)
{
	T reciprocal = (T)1.0 / scale;
	x *= reciprocal;
	y *= reciprocal;
	return *this;
}

//---------------------------------------------------------------
template<typename T>
inline T*
Vector2<T>::Ptr()
{
	return &x;
}
//---------------------------------------------------------------
template<typename T>
inline const T*
Vector2<T>::Ptr() const
{
	return &x;
}
//---------------------------------------------------------------
template<typename T>
inline T
Vector2<T>::LengthSQ()	const
{
	return x * x + y * y;
}
//---------------------------------------------------------------
template<typename T>
inline T
Vector2<T>::Length()	const
{
	return Math<T>::Sqrt(x * x + y * y);
}
//---------------------------------------------------------------
template<typename T>
inline void
Vector2<T>::Normalize()
{
	T len = Length();
	if (len > Math<T>::ZERO_TOLERANCE)
	{
		T invLen = 1.0f / len;
		x *= invLen;
		y *= invLen;
	}
	else
	{
		x = y = (T)0.0;
	}
}

//---------------------------------------------------------------
template<typename T>
inline Vector2<T> operator / (T a, const Vector2<T> v)
{
	return Vector2<T>(a / v.x, a / v.y);
}


template<typename T>
inline T Vector2<T>::Dot(const Vector2<T>& rhs) const
{
	return x * rhs.x +
		y * rhs.y;
}
//---------------------------------------------------------------
template<typename T>
inline T
Vector2<T>::Dot(const Vector2<T>& lhs, const Vector2<T>& rhs)
{
	return lhs.x * rhs.x +
		lhs.y * rhs.y;
}

template<typename T>
Vector2<T> Vector2<T>::Lerp(const Vector2<T>& a, const Vector2<T>& b, T s)
{
	return Vector2<T>(
		a.x + s * (b.x - a.x),
		a.y + s * (b.y - a.y));
}

typedef	Vector2<float>	Vector2f;

//////////////////////////////////////////////////////////////////////////

template<typename T>
Vector3<T>::Vector3()	{}
//---------------------------------------------------------------
template<typename T>
Vector3<T>::Vector3(T fx, T fy, T fz)
{
	x = fx;
	y = fy;
	z = fz;
}
//---------------------------------------------------------------
template<typename T>
Vector3<T>::Vector3(const Vector3<T>& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

template<typename T>
Vector3<T>::Vector3(const T v[3])
{
	x = v[0];
	y = v[1];
	z = v[2];
}
//---------------------------------------------------------------
template<typename T>
inline const Vector3<T>&
Vector3<T>::operator = (const Vector3<T>& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline void
Vector3<T>::Set(T fx, T fy, T fz)
{
	x = fx; y = fy; z = fz;
}
//---------------------------------------------------------------
template<typename T>
inline void
Vector3<T>::Set(const T* ptr)
{
	x = ptr[0];	y = ptr[1];	z = ptr[2];
}
//---------------------------------------------------------------
template<typename T>
inline T& Vector3<T>::operator [](size_t index)
{
	return *(&x + index);
}
//---------------------------------------------------------------
template<typename T>
inline const T& Vector3<T>::operator[](size_t index) const
{
	return *(&x + index);
}
//---------------------------------------------------------------
template<typename T>
inline Vector3<T> Vector3<T>::operator -() const
{
	return Vector3<T>(-x, -y, -z);
}
//---------------------------------------------------------------
template<typename T>
inline Vector3<T>
Vector3<T>::operator +(const Vector3<T>& v) const
{
	return Vector3<T>(x + v.x, y + v.y, z + v.z);
}
//---------------------------------------------------------------
template<typename T>
inline Vector3<T>
Vector3<T>::operator -(const Vector3<T>& v) const
{
	return Vector3<T>(x - v.x, y - v.y, z - v.z);
}
//---------------------------------------------------------------
template<typename T>
inline Vector3<T>
Vector3<T>::operator *(T scale) const
{
	return Vector3<T>(x*scale, y*scale, z*scale);
}
//---------------------------------------------------------------
template<typename T>
inline Vector3<T>
Vector3<T>::operator /(T scale) const
{
	T reciprocal = (T)1.0 / scale;
	return Vector3<T>(x*reciprocal, y*reciprocal, z*reciprocal);
}
//---------------------------------------------------------------
template<typename T>
inline const Vector3<T>&
Vector3<T>::operator +=(const Vector3<T>& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Vector3<T>&
Vector3<T>::operator -=(const Vector3<T>& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Vector3<T>&
Vector3<T>::operator *=(T scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Vector3<T>&
Vector3<T>::operator /=(T scale)
{
	T reciprocal = (T)1.0 / scale;
	x *= reciprocal;
	y *= reciprocal;
	z *= reciprocal;
	return *this;
}

//---------------------------------------------------------------
template<typename T>
inline T*
Vector3<T>::Ptr()
{
	return &x;
}
//---------------------------------------------------------------
template<typename T>
inline const T*
Vector3<T>::Ptr() const
{
	return &x;
}
//---------------------------------------------------------------
template<typename T>
inline T
Vector3<T>::LengthSQ()	const
{
	return x * x + y * y + z * z;
}
//---------------------------------------------------------------
template<typename T>
inline T
Vector3<T>::Length()	const
{
	return Math<T>::Sqrt(x * x + y * y + z * z);
}
//---------------------------------------------------------------
template<typename T>
inline void
Vector3<T>::Normalize()
{
	T len = Length();
	if (len > Math<T>::ZERO_TOLERANCE)
	{
		T invLen = (T)1.0 / len;
		x *= invLen;
		y *= invLen;
		z *= invLen;
	}
	else
	{
		x = y = z = (T)0.0;
	}
}

//---------------------------------------------------------------
template<typename T>
inline Vector3<T> operator / (T a, const Vector3<T> v)
{
	return Vector3<T>(a / v.x, a / v.y, a / v.z);
}
//---------------------------------------------------------------
template<typename T>
T Vector3<T>::Dot(const Vector3<T>& l, const Vector3<T>& r)
{
	return l.x * r.x +
		l.y * r.y +
		l.z * r.z;
}

template<typename T>
inline T Vector3<T>::Dot(const Vector3<T>& v) const
{
	return Vector3<T>::Dot(*this, v);
}

template<typename T>
Vector3<T> Vector3<T>::Cross(const Vector3<T>& l, const Vector3<T>& r)
{
	return Vector3<T>(l.y * r.z - l.z * r.y,
		l.z * r.x - l.x * r.z,
		l.x * r.y - l.y * r.x);
}

template<typename T>
inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& v) const
{
	return Vector3<T>::Cross(*this, v);
}

template<typename T>
Vector3<T> Vector3<T>::Lerp(const Vector3<T>& a, const Vector3<T>& b, T s)
{
	return Vector3<T>(
		a.x + s * (b.x - a.x),
		a.y + s * (b.y - a.y),
		a.z + s * (b.z - a.z));
}

//---------------------------------------------------------------
template<typename T>
inline Vector3<T>
Modulate(const Vector3<T>& lhs, const Vector3<T>& rhs)
{
	return Vector3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}
//--------------------------------------------------------------------------

template<>
inline std::string Vector3<float>::ToString() const
{
	char buff[64];
	sprintf(buff, "%f,%f,%f", x, y, z);
	std::string s = buff;
	return s;
}

typedef	Vector3<float>	Vector3f;

//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------
template<typename T>
Vector4<T>::Vector4()	{}
//---------------------------------------------------------------
template<typename T>
Vector4<T>::Vector4(T fx, T fy, T fz, T fw)
{
	x = fx;
	y = fy;
	z = fz;
	w = fw;
}
//---------------------------------------------------------------
template<typename T>
Vector4<T>::Vector4(const Vector4<T>& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}
//---------------------------------------------------------------
template<typename T>
inline const Vector4<T>&
Vector4<T>::operator = (const Vector4<T>& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline void
Vector4<T>::Set(T fx, T fy, T fz, T fw)
{
	x = fx; y = fy; z = fz; w = fw;
}
//---------------------------------------------------------------
template<typename T>
inline void
Vector4<T>::Set(const T* ptr)
{
	x = ptr[0];	y = ptr[1];	z = ptr[2]; w = ptr[3];
}
//---------------------------------------------------------------
template<typename T>
inline T& Vector4<T>::operator [](size_t index)
{
	return *(&x + index);
}
//---------------------------------------------------------------
template<typename T>
inline const T& Vector4<T>::operator[](size_t index) const
{
	return *(&x + index);
}
//---------------------------------------------------------------
template<typename T>
inline Vector4<T> Vector4<T>::operator -() const
{
	return Vector4<T>(-x, -y, -z, -w);
}
//---------------------------------------------------------------
template<typename T>
inline Vector4<T>
Vector4<T>::operator +(const Vector4<T>& v) const
{
	return Vector4<T>(x + v.x, y + v.y, z + v.z, w + v.w);
}
//---------------------------------------------------------------
template<typename T>
inline Vector4<T>
Vector4<T>::operator -(const Vector4<T>& v) const
{
	return Vector4<T>(x - v.x, y - v.y, z - v.z, w - v.w);
}
//---------------------------------------------------------------
template<typename T>
inline Vector4<T>
Vector4<T>::operator *(T scale) const
{
	return Vector4<T>(x*scale, y*scale, z*scale, w*scale);
}
//---------------------------------------------------------------
template<typename T>
inline Vector4<T>
Vector4<T>::operator /(T scale) const
{
	T reciprocal = (T)1.0 / scale;
	return Vector4<T>(x*reciprocal, y*reciprocal, z*reciprocal, w*reciprocal);
}
//---------------------------------------------------------------
template<typename T>
inline const Vector4<T>&
Vector4<T>::operator +=(const Vector4<T>& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Vector4<T>&
Vector4<T>::operator -=(const Vector4<T>& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Vector4<T>&
Vector4<T>::operator *=(T scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Vector4<T>&
Vector4<T>::operator /=(T scale)
{
	T reciprocal = (T)1.0 / scale;
	x *= reciprocal;
	y *= reciprocal;
	z *= reciprocal;
	w *= reciprocal;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline Vector3<T> Vector4<T>::ProjectToVector3() const
{
	T reciprocal = (T)1.0 / w;
	return Vector3<T>(x * reciprocal, y * reciprocal, z * reciprocal);
}
//---------------------------------------------------------------
template<typename T>
inline T*
Vector4<T>::Ptr()
{
	return &x;
}
//---------------------------------------------------------------
template<typename T>
inline const T*
Vector4<T>::Ptr() const
{
	return &x;
}
//---------------------------------------------------------------
template<typename T>
inline T
Vector4<T>::LengthSQ()	const
{
	return x * x + y * y + z * z + w * w;
}
//---------------------------------------------------------------
template<typename T>
inline T
Vector4<T>::Length()	const
{
	return Math<T>::Sqrt(x * x + y * y + z * z + w * w);
}
//---------------------------------------------------------------
template<typename T>
inline void
Vector4<T>::Normalize()
{
	T len = Length();
	if (len > Math<T>::ZERO_TOLERANCE)
	{
		T invLen = (T)1.0 / len;
		x *= invLen;
		y *= invLen;
		z *= invLen;
		w *= invLen;
	}
	else
	{
		x = y = z = w = (T)0.0;
	}
}

template<typename T>
T Vector4<T>::Dot(const Vector4<T>& lhs, const Vector4<T>& rhs)
{
	return lhs.x * rhs.x +
		lhs.y * rhs.y +
		lhs.z * rhs.z +
		lhs.w * rhs.w;
}

template<typename T>
inline T Vector4<T>::Dot(const Vector4<T>& rhs) const
{
	return Vector4<T>::Dot(*this, rhs);
}

template<typename T>
inline std::string Vector4<T>::ToString() const
{
	return std::string();
}

template<typename T>
Vector3<T> Vector4<T>::FromString(const char* str)
{
	return  Vector4<T>::ZERO;
}

//---------------------------------------------------------------
template<typename T>
inline Vector4<T>
Modulate(const Vector4<T>& lhs, const Vector4<T>& rhs)
{
	return Vector4<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

typedef	Vector4<float>	Vector4f;


//////////////////////////////////////////////////////////////////////////


template<typename real>
Matrix3<real>::Matrix3()
{
}
//-----------------------------------------------------------
template<typename real>
Matrix3<real>::Matrix3(const Matrix3<real>& matrix)
{
	memcpy(m, matrix.m, sizeof(m));
}

//-----------------------------------------------------------
template<typename real>
Matrix3<real>::Matrix3(const real* data)
{
	memcpy(m, data, sizeof(m));
}
//-----------------------------------------------------------
template<typename real>
Matrix3<real>::Matrix3(real m11, real m12, real m13,
	real m21, real m22, real m23,
	real m31, real m32, real m33)
{
	_11 = m11;	_12 = m12;	_13 = m13;
	_21 = m21;	_22 = m22;	_23 = m23;
	_31 = m31;	_32 = m32;	_33 = m33;
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix3<real>&
Matrix3<real>::operator=(const Matrix3<real>& rhl)
{
	memcpy(m, rhl.m, sizeof(m));
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline void
Matrix3<real>::Set(real m11, real m12, real m13,
real m21, real m22, real m23,
real m31, real m32, real m33)
{
	_11 = m11;	_12 = m12;	_13 = m13;
	_21 = m21;	_22 = m22;	_23 = m23;
	_31 = m31;	_32 = m32;	_33 = m33;
}
//-----------------------------------------------------------
template<typename real>
inline real*
Matrix3<real>::Ptr()
{
	return m;
}
//-----------------------------------------------------------
template<typename real>
inline const real*
Matrix3<real>::Ptr() const
{
	return m;
}
//-------------------------------------------------------------
template<typename real>
inline real*
Matrix3<real>::GetRow(size_t row) const
{
	return &m[row * 3];
}
//-------------------------------------------------------------
template<typename real>
inline void
Matrix3<real>::TransposeSelf()
{
	real temp;
	temp = _12; _12 = _21; _21 = temp;
	temp = _13; _13 = _31; _31 = temp;

	temp = _23; _23 = _32; _32 = temp;
}

template<typename real>
Matrix4<real> Matrix3<real>::ToMat4() const
{
	return Matrix4<real>(
		_11, _12, _13, 0.f,
		_21, _22, _23, 0.f,
		_31, _32, _33, 0.f,
		0.f, 0.f, 0.f, 1.f);
}

template<typename real>
Matrix3<real> Matrix3<real>::operator * (const Matrix3<real>& r)
{
	Matrix3<real> out(
		/*out._11 = */_11 * r._11 + _12 * r._21 + _13 * r._31,
		/*out._12 = */_11 * r._12 + _12 * r._22 + _13 * r._32,
		/*out._13 = */_11 * r._13 + _12 * r._23 + _13 * r._33,

		/*out._21 = */_21 * r._11 + _22 * r._21 + _23 * r._31,
		/*out._22 = */_21 * r._12 + _22 * r._22 + _23 * r._32,
		/*out._23 = */_21 * r._13 + _22 * r._23 + _23 * r._33,

		/*out._31 = */_31 * r._11 + _32 * r._21 + _33 * r._31,
		/*out._32 = */_31 * r._12 + _32 * r._22 + _33 * r._32,
		/*out._33 = */_31 * r._13 + _32 * r._23 + _33 * r._33);
	return out;
}

template<typename real>
Matrix3<real>& Matrix3<real>::operator *= (const Matrix3<real>& m)
{
	*this = *this * m;
	return *this;
}

template<typename U>
Vector3<U> operator * (const Vector3<U>& v, const Matrix3<U>& m)
{
	Vector3<U> out(v.x*m._11 + v.y*m._21 + v.z*m._31,
		v.x*m._12 + v.y*m._22 + v.z*m._32,
		v.x*m._13 + v.y*m._23 + v.z*m._33);
	return out;
}

template<typename U>
Vector3<U>&	operator *= (Vector3<U>& v, const Matrix3<U>& m)
{
	v = v * m;
	return v;
}

typedef Matrix3<float> Matrix3f;

//-----------------------------------------------------------------
template<typename real>
Matrix4<real>::Matrix4(const Matrix4<real>& matrix)
{
	memcpy(m, matrix.m, sizeof(m));
}

//-----------------------------------------------------------------
template<typename real>
Matrix4<real>::Matrix4(const real* data)
{
	memcpy(m, data, sizeof(m));
}
//-----------------------------------------------------------------
template<typename real>
Matrix4<real>::Matrix4(real m11, real m12, real m13, real m14,
	real m21, real m22, real m23, real m24,
	real m31, real m32, real m33, real m34,
	real m41, real m42, real m43, real m44)
{
	_11 = m11;	_12 = m12;	_13 = m13;	_14 = m14;
	_21 = m21;	_22 = m22;	_23 = m23;	_24 = m24;
	_31 = m31;	_32 = m32;	_33 = m33;	_34 = m34;
	_41 = m41;	_42 = m42;	_43 = m43;	_44 = m44;
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix4<real>&
Matrix4<real>::operator=(const Matrix4<real>& rhl)
{
	memcpy(m, rhl.m, sizeof(m));
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline void
Matrix4<real>::Set(real m11, real m12, real m13, real m14,
real m21, real m22, real m23, real m24,
real m31, real m32, real m33, real m34,
real m41, real m42, real m43, real m44)
{
	_11 = m11;	_12 = m12;	_13 = m13;	_14 = m14;
	_21 = m21;	_22 = m22;	_23 = m23;	_24 = m24;
	_31 = m31;	_32 = m32;	_33 = m33;	_34 = m34;
	_41 = m41;	_42 = m42;	_43 = m43;	_44 = m44;
}
//-----------------------------------------------------------------
template<typename real>
inline Matrix4<real>
Matrix4<real>::operator +(const Matrix4<real>& m) const
{
	return Matrix4<real>(
		_11 + m._11, _12 + m._12, _13 + m._13, _14 + m._14,
		_21 + m._21, _22 + m._22, _23 + m._23, _24 + m._24,
		_31 + m._31, _32 + m._32, _33 + m._33, _34 + m._34,
		_41 + m._41, _42 + m._42, _43 + m._43, _44 + m._44);

}
//-----------------------------------------------------------------
template<typename real>
inline Matrix4<real>
Matrix4<real>::operator -(const Matrix4<real>& m) const
{
	return Matrix4<real>(
		_11 - m._11, _12 - m._12, _13 - m._13, _14 - m._14,
		_21 - m._21, _22 - m._22, _23 - m._23, _24 - m._24,
		_31 - m._31, _32 - m._32, _33 - m._33, _34 - m._34,
		_41 - m._41, _42 - m._42, _43 - m._43, _44 - m._44);
}
//-----------------------------------------------------------------
template<typename real>
inline Matrix4<real>
Matrix4<real>::operator *(real scale) const
{
	return Matrix4<real>(
		_11 * scale, _12 * scale, _13 * scale, _14 * scale,
		_21 * scale, _22 * scale, _23 * scale, _24 * scale,
		_31 * scale, _32 * scale, _33 * scale, _34 * scale,
		_41 * scale, _42 * scale, _43 * scale, _44 * scale);
}
//-----------------------------------------------------------------
template<typename real>
inline Matrix4<real>
Matrix4<real>::operator /(real scale) const
{
	real inv = (real)1.0 / scale;
	return Matrix4<real>(
		_11 * inv, _12 * inv, _13 * inv, _14 * inv,
		_21 * inv, _22 * inv, _23 * inv, _24 * inv,
		_31 * inv, _32 * inv, _33 * inv, _34 * inv,
		_41 * inv, _42 * inv, _43 * inv, _44 * inv);
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix4<real>&
Matrix4<real>::operator +=(Matrix4<real>& m)
{
	_11 += m._11;	_12 += m._12;	_13 += m._13;	_14 += m._14;
	_21 += m._21;	_22 += m._22;	_23 += m._23;	_24 += m._24;
	_31 += m._31;	_32 += m._32;	_33 += m._33;	_34 += m._34;
	_41 += m._41;	_42 += m._42;	_43 += m._43;	_44 += m._44;
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix4<real>&
Matrix4<real>::operator -=(Matrix4<real>& m)
{
	_11 -= m._11;	_12 -= m._12;	_13 -= m._13;	_14 -= m._14;
	_21 -= m._21;	_22 -= m._22;	_23 -= m._23;	_24 -= m._24;
	_31 -= m._31;	_32 -= m._32;	_33 -= m._33;	_34 -= m._34;
	_41 -= m._41;	_42 -= m._42;	_43 -= m._43;	_44 -= m._44;
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix4<real>&
Matrix4<real>::operator *=(real scale)
{
	for (int i = 0; i < 16; i++)
	{
		m[i] *= scale;
	}
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline const Matrix4<real>&
Matrix4<real>::operator /=(real scale)
{
	real inv = (real)1.0 / scale;
	for (int i = 0; i < 16; i++)
	{
		m[i] *= inv;
	}
	return *this;
}
//-----------------------------------------------------------------
template<typename real>
inline real*
Matrix4<real>::Ptr()
{
	return m;
}
//-----------------------------------------------------------------
template<typename real>
inline const real*
Matrix4<real>::Ptr() const
{
	return m;
}
//-----------------------------------------------------------------
template<typename real>
inline real*
Matrix4<real>::GetRow(size_t row) const
{
	return &m[row * 4];
}
//-----------------------------------------------------------------
template<typename real>
inline void
Matrix4<real>::TransposeSelf()
{
	real temp;
	temp = _12; _12 = _21; _21 = temp;
	temp = _13; _13 = _31; _31 = temp;
	temp = _14; _14 = _41; _41 = temp;

	temp = _23; _23 = _32; _32 = temp;
	temp = _24; _24 = _42; _42 = temp;

	temp = _34; _34 = _43; _43 = temp;
}


template<typename real>
Matrix4<real> Matrix4<real>::operator * (const Matrix4<real>& r)
{
	Matrix4 out(
		/*out._11 = */_11 * r._11 + _12 * r._21 + _13 * r._31 + _14 * r._41,
		/*out._12 = */_11 * r._12 + _12 * r._22 + _13 * r._32 + _14 * r._42,
		/*out._13 = */_11 * r._13 + _12 * r._23 + _13 * r._33 + _14 * r._43,
		/*out._14 = */_11 * r._14 + _12 * r._24 + _13 * r._34 + _14 * r._44,

		/*out._21 = */_21 * r._11 + _22 * r._21 + _23 * r._31 + _24 * r._41,
		/*out._22 = */_21 * r._12 + _22 * r._22 + _23 * r._32 + _24 * r._42,
		/*out._23 = */_21 * r._13 + _22 * r._23 + _23 * r._33 + _24 * r._43,
		/*out._24 = */_21 * r._14 + _22 * r._24 + _23 * r._34 + _24 * r._44,

		/*out._31 = */_31 * r._11 + _32 * r._21 + _33 * r._31 + _34 * r._41,
		/*out._32 = */_31 * r._12 + _32 * r._22 + _33 * r._32 + _34 * r._42,
		/*out._33 = */_31 * r._13 + _32 * r._23 + _33 * r._33 + _34 * r._43,
		/*out._34 = */_31 * r._14 + _32 * r._24 + _33 * r._34 + _34 * r._44,

		/*out._41 = */_41 * r._11 + _42 * r._21 + _43 * r._31 + _44 * r._41,
		/*out._42 = */_41 * r._12 + _42 * r._22 + _43 * r._32 + _44 * r._42,
		/*out._43 = */_41 * r._13 + _42 * r._23 + _43 * r._33 + _44 * r._43,
		/*out._44 = */_41 * r._14 + _42 * r._24 + _43 * r._34 + _44 * r._44);
	return out;
}

template<typename real>
Matrix4<real>& Matrix4<real>::operator *= (const Matrix4<real>& m)
{
	*this = *this * m;
	return *this;
}

template<typename U>
Vector3<U> operator * (const Vector3<U>& v, const Matrix4<U>& m)
{
	Vector3<U> out(
		v.x*m._11 + v.y*m._21 + v.z*m._31 + m._41,
		v.x*m._12 + v.y*m._22 + v.z*m._32 + m._42,
		v.x*m._13 + v.y*m._23 + v.z*m._33 + m._43);
	return out;
}

template<typename U>
Vector3<U>& operator *= (Vector3<U>& v, const Matrix4<U>& m)
{
	v = v * m;
	return v;
}

template<typename real>
Matrix4<real> Matrix4<real>::Inverse() const
{
	real fA0 = m[0] * m[5] - m[1] * m[4];
	real fA1 = m[0] * m[6] - m[2] * m[4];
	real fA2 = m[0] * m[7] - m[3] * m[4];
	real fA3 = m[1] * m[6] - m[2] * m[5];
	real fA4 = m[1] * m[7] - m[3] * m[5];
	real fA5 = m[2] * m[7] - m[3] * m[6];
	real fB0 = m[8] * m[13] - m[9] * m[12];
	real fB1 = m[8] * m[14] - m[10] * m[12];
	real fB2 = m[8] * m[15] - m[11] * m[12];
	real fB3 = m[9] * m[14] - m[10] * m[13];
	real fB4 = m[9] * m[15] - m[11] * m[13];
	real fB5 = m[10] * m[15] - m[11] * m[14];

	real fDet = fA0*fB5 - fA1*fB4 + fA2*fB3 + fA3*fB2 - fA4*fB1 + fA5*fB0;
	if (Math<real>::Abs(fDet) <= Math<real>::ZERO_TOLERANCE)
	{
		return Matrix4<real>::ZERO;
	}

	return Matrix4<real>(
		+m[5] * fB5 - m[6] * fB4 + m[7] * fB3,
		-m[1] * fB5 + m[2] * fB4 - m[3] * fB3,
		+m[13] * fA5 - m[14] * fA4 + m[15] * fA3,
		-m[9] * fA5 + m[10] * fA4 - m[11] * fA3,
		-m[4] * fB5 + m[6] * fB2 - m[7] * fB1,
		+m[0] * fB5 - m[2] * fB2 + m[3] * fB1,
		-m[12] * fA5 + m[14] * fA2 - m[15] * fA1,
		+m[8] * fA5 - m[10] * fA2 + m[11] * fA1,
		+m[4] * fB4 - m[5] * fB2 + m[7] * fB0,
		-m[0] * fB4 + m[1] * fB2 - m[3] * fB0,
		+m[12] * fA4 - m[13] * fA2 + m[15] * fA0,
		-m[8] * fA4 + m[9] * fA2 - m[11] * fA0,
		-m[4] * fB3 + m[5] * fB1 - m[6] * fB0,
		+m[0] * fB3 - m[1] * fB1 + m[2] * fB0,
		-m[12] * fA3 + m[13] * fA1 - m[14] * fA0,
		+m[8] * fA3 - m[9] * fA1 + m[10] * fA0);

	//	out /= fDet;
}

template<typename real>
const Matrix4<real>& Matrix4<real>::InverseSelf()
{
	*this = Inverse();
}

template<typename real>
Quaternion<real> Matrix4<real>::ToQuaternion() const
{
	Quaternion<real> out;

	const real* mat = this->m;

	real T = mat[0] + mat[5] + mat[10];

	if (T > (real)0.0)
	{
		real S = Math<real>::Sqrt((real)1.0 + T) * (real)2;
		out.x = (mat[6] - mat[9]) / S;
		out.y = (mat[8] - mat[2]) / S;
		out.z = (mat[1] - mat[4]) / S;
		out.w = (real)0.25 * S;
	}
	else if (mat[0] > mat[5] && mat[0] > mat[10])			// Column 0: 
	{
		real S = Math<real>::Sqrt((real)1.0 + mat[0] - mat[5] - mat[10]) * (real)2;
		out.x = (real)0.25 * S;
		out.y = (mat[4] + mat[1]) / S;
		out.z = (mat[2] + mat[8]) / S;
		out.w = (mat[6] - mat[9]) / S;
	}
	else if (mat[5] > mat[10])						// Column 1: 
	{
		real S = Math<real>::Sqrt((real)1.0 + mat[5] - mat[0] - mat[10]) * (real)2;
		out.x = (mat[4] + mat[1]) / S;
		out.y = (real)0.25 * S;
		out.z = (mat[9] + mat[6]) / S;
		out.w = (mat[8] - mat[2]) / S;
	}
	else												// Column 2:
	{
		real S = Math<real>::Sqrt((real)1.0 + mat[10] - mat[0] - mat[5]) * (real)2;
		out.x = (mat[2] + mat[8]) / S;
		out.y = (mat[9] + mat[6]) / S;
		out.z = (real)0.25 * S;
		out.w = (mat[1] - mat[4]) / S;
	}

	return out;
}

template<typename real>
Matrix4<real> Matrix4<real>::RotateX(float angle)
{
	Matrix4<real> mat = Matrix4<real>::IDENTITY;
	real spitch = Math<real>::Sin(angle);
	real cpitch = Math<real>::Cos(angle);

	mat._22 = cpitch;
	mat._23 = spitch;
	mat._32 = -spitch;
	mat._33 = cpitch;
	return mat;
}

template<typename real>
Matrix4<real> Matrix4<real>::RotateY(float angle)
{
	Matrix4<real> mat = Matrix4<real>::IDENTITY;
	real syaw = Math<real>::Sin(angle);
	real cyaw = Math<real>::Cos(angle);

	mat._11 = cyaw;
	mat._13 = -syaw;
	mat._31 = syaw;
	mat._33 = cyaw;
	return mat;
}

template<typename real>
Matrix4<real> Matrix4<real>::RotateZ(float angle)
{
	Matrix4<real> mat = Matrix4<real>::IDENTITY;
	real sroll = Math<real>::Sin(angle);
	real croll = Math<real>::Cos(angle);

	mat._11 = croll;
	mat._12 = sroll;
	mat._21 = -sroll;
	mat._22 = croll;
	return mat;
}

template<typename real>
Matrix4<real> Matrix4<real>::Scale(const Vector3<real>& s)
{
	Matrix4<real> mat = Matrix4<real>::IDENTITY;
	mat._11 = s.x;
	mat._22 = s.y;
	mat._33 = s.z;
	return mat;
}

template<typename real>
Matrix4<real> Matrix4<real>::RotationAxis(const Vector3<real>& axis, real angle)
{
	Vector3<real> v = axis;
	v.Normalize();

	real c = Math<real>::Cos(angle);
	real s = Math<real>::Sin(angle);
	real minusCos = 1 - c;
	real x2 = v.x * v.x;
	real y2 = v.y * v.y;
	real z2 = v.z * v.z;
	real xy = v.x * v.y;
	real xz = v.x * v.z;
	real yz = v.y * v.z;

	return Matrix4<real>(
	x2 * minusCos + c,		xy * minusCos + v.z * s,	xz * minusCos - v.y * s,	 (real)0.0,
	xy * minusCos - v.z * s,	y2 * minusCos + c,		yz * minusCos + v.x * s,	 (real)0.0,
	xz * minusCos + v.y * s,	yz * minusCos - v.x * s,	z2 * minusCos + c,		 (real)0.0,
	(real)0.0,				(real)0.0,				(real)0.0,				 (real)1.0);
}

template<typename real>
Matrix4<real> Matrix4<real>::Translate(const Vector3<real>& t)
{
	Matrix4<real> mat = Matrix4<real>::IDENTITY;
	mat._41 = t.x;
	mat._42 = t.y;
	mat._43 = t.z;
	return mat;
}

// matrix from rotation and translation
template<typename real>
Matrix4<real> Matrix4<real>::Transform(const Quaternion<real>& q, const Vector3<real>& t)
{
	Matrix4<real> mat = q.ToMat4();
	mat._41 = t.x;
	mat._42 = t.y;
	mat._43 = t.z;

	return mat;
}

template<typename real>
Matrix4<real> Matrix4<real>::LookAtLH(const Vector3<real>& vEye, const Vector3<real>& vAt, const Vector3<real>& vUp)
{
	/*	zaxis = normal(At - Eye)
	xaxis = normal(cross(Up, zaxis))
	yaxis = cross(zaxis, xaxis)

	xaxis.x           yaxis.x           zaxis.x          0
	xaxis.y           yaxis.y           zaxis.y          0
	xaxis.z           yaxis.z           zaxis.z          0
	-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  l*/

	Vector3<real> z = vAt - vEye;
	z.Normalize();
	Vector3<real> x = Vector3f::Cross(vUp, z);
	x.Normalize();
	Vector3<real> y = Vector3f::Cross(z, x);
	return Matrix4<real>(x.x, y.x, z.x, 0.0f,
		x.y, y.y, z.y, 0.0f,
		x.z, y.z, z.z, 0.0f,
		-Vector3f::Dot(x, vEye), -Vector3f::Dot(y, vEye), -Vector3f::Dot(z, vEye), 1.0f);
}

template<typename real>
Matrix4<real> Matrix4<real>::LookAtRH(const Vector3<real>& vEye, const Vector3<real>& vAt, const Vector3<real>& vUp)
{
	/*	zaxis = normal(At - Eye)
	xaxis = normal(cross(Up, zaxis))
	yaxis = cross(zaxis, xaxis)

	xaxis.x           yaxis.x           zaxis.x          0
	xaxis.y           yaxis.y           zaxis.y          0
	xaxis.z           yaxis.z           zaxis.z          0
	-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  l*/

	Vector3<real> z = vEye - vAt;
	z.Normalize();
	Vector3<real> x = Vector3f::Cross(vUp, z);
	x.Normalize();
	Vector3<real> y = Vector3f::Cross(z, x);
	return Matrix4<real>(x.x, y.x, z.x, 0.0f,
		x.y, y.y, z.y, 0.0f,
		x.z, y.z, z.z, 0.0f,
		-Vector3f::Dot(x, vEye), -Vector3f::Dot(y, vEye), -Vector3f::Dot(z, vEye), 1.0f);
}

template<typename real>
Matrix4<real> Matrix4<real>::PerspectiveFovLH(real fovy, real aspect, real zn, real zf)
{
	/*
	xScale     0          0               0
	0        yScale       0               0
	0          0       zf/(zf-zn)         1
	0          0       -zn*zf/(zf-zn)     0
	where:
	yScale = cot(fovY/2)

	xScale = yScale / aspect ratio
	*/

	real yScale = 1.0f / Math<real>::Tan(fovy / 2.0f);
	real xScale = yScale / aspect;
	real zScale = zf / (zf - zn);

	return Matrix4<real>(xScale, 0.0f, 0.0f, 0.0f,
		0.0f, yScale, 0.0f, 0.0f,
		0.0f, 0.0f, zScale, 1.0f,
		0.0f, 0.0f, -zn * zScale, 0.0f);
}
template<typename real>
Matrix4<real> Matrix4<real>::PerspectiveFovRH(real fovy, real aspect, real zn, real zf)
{

	real yScale = 1.0f / Math<real>::Tan(fovy / 2.0f);
	real xScale = yScale / aspect;
	real zScale = zf / (zn - zf);

	return Matrix4<real>(xScale, 0.0f, 0.0f, 0.0f,
		0.0f, yScale, 0.0f, 0.0f,
		0.0f, 0.0f, zScale, -1.0f,
		0.0f, 0.0f, zn * zScale, 0.0f);
}

template<typename real>
Matrix4<real> Matrix4<real>::OrthoLH(real width, real height, real zn, real zf)
{
	real InvDepth = 1.0f / (zf - zn);

	return Matrix4<real>(2.0f / width, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / height, 0.0f, 0.0f,
		0.0f, 0.0f, InvDepth, 0.0f,
		0.0f, 0.0f, -zn * InvDepth, 1.0f);
}
template<typename real>
Matrix4<real> Matrix4<real>::OrthoRH(real width, real height, real zn, real zf)
{
	real InvDepth = 1.0f / (zn - zf);

	return Matrix4<real>(2.0f / width, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / height, 0.0f, 0.0f,
		0.0f, 0.0f, InvDepth, 0.0f,
		0.0f, 0.0f, zn * InvDepth, 1.0f);
}

template<typename real>
Matrix4<real> Matrix4<real>::OrthoOffCenterLH(real l, real r, real b, real t, real zn, real zf)
{
	real InvDepth = 1.0f / (zf - zn);

	return Matrix4<real>(2.0f / (r - l), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (t - b), 0.0f, 0.0f,
		0.0f, 0.0f, InvDepth, 0.0f,
		(l + r) / (l - r), (t + b) / (b - t), -zn * InvDepth, 1.0f);
}
template<typename real>
Matrix4<real> Matrix4<real>::OrthoOffCenterRH(real l, real r, real b, real t, real zn, real zf)
{
	real InvDepth = 1.0f / (zn - zf);

	return Matrix4<real>(2.0f / (r - l), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (t - b), 0.0f, 0.0f,
		0.0f, 0.0f, InvDepth, 0.0f,
		(l + r) / (l - r), (t + b) / (b - t), zn * InvDepth, 1.0f);
}


typedef Matrix4<float> Matrix4f;

//////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------
template<typename T>
Quaternion<T>::Quaternion(T fw, T fx, T fy, T fz)
{
	w = fw;		x = fx;		y = fy;		z = fz;
}
//-------------------------------------------------------------------
template<typename T>
Quaternion<T>::Quaternion(const Quaternion<T>& q)
{
	w = q.w;	x = q.x;	y = q.y;	z = q.z;
}
//-------------------------------------------------------------------
template<typename T>
inline const Quaternion<T>&
Quaternion<T>::operator = (const Quaternion<T>& q)
{
	w = q.w;	x = q.x;	y = q.y;	z = q.z;
	return *this;
}
//-------------------------------------------------------------------
template<typename T>
Quaternion<T>::Quaternion(T fx, T fy, T fz)
{
	x = fx;	y = fy;	z = fz;
	w = Math<T>::Sqrt((T)1.0 - x * x - y * y - z * z);
}
//-------------------------------------------------------------------
template<typename T>
inline void
Quaternion<T>::Set(T fw, T fx, T fy, T fz)
{
	w = fw;		x = fx;		y = fy;		z = fz;
}
//-------------------------------------------------------------------
template<typename T>
inline void
Quaternion<T>::Set(const T* ptr)
{
	w = ptr[0];	x = ptr[1];	y = ptr[2];	z = ptr[3];
}
//-------------------------------------------------------------------
template<typename T>
inline void
Quaternion<T>::Set(T fx, T fy, T fz)
{
	x = fx;	y = fy;	z = fz;
	w = Math<T>::Sqrt((T)1.0 - x * x - y * y - z * z);
}
//---------------------------------------------------------------
template<typename T>
inline T
Quaternion<T>::LengthSQ()	const
{
	return w * w + x * x + y * y + z * z;
}
//---------------------------------------------------------------
template<typename T>
inline Quaternion<T>
Quaternion<T>::operator -() const
{
	return Quaternion<T>(-w, -x, -y, -z);
}
//---------------------------------------------------------------
template<typename T>
inline Quaternion<T>
Quaternion<T>::operator +(const Quaternion<T>& q) const
{
	return Quaternion<T>(w + q.w, x + q.x, y + q.y, z + q.z);
}
//---------------------------------------------------------------
template<typename T>
inline Quaternion<T>
Quaternion<T>::operator -(const Quaternion<T>& q) const
{
	return Quaternion<T>(w - q.w, x - q.x, y - q.y, z - q.z);
}
//---------------------------------------------------------------
template<typename T>
inline Quaternion<T>
Quaternion<T>::operator *(T scale) const
{
	return Quaternion<T>(w * scale, x * scale, y * scale, z * scale);
}
//---------------------------------------------------------------
template<typename T>
inline Quaternion<T>
Quaternion<T>::operator /(T scale) const
{
	T inv = (T)1.0 / scale;
	return Quaternion<T>(w * inv, x * inv, y * inv, z * inv);
}
//---------------------------------------------------------------
template<typename T>
inline const Quaternion<T>&
Quaternion<T>::operator +=(const Quaternion<T>& q)
{
	w += q.w;
	x += q.x;
	y += q.y;
	z += q.z;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Quaternion<T>&
Quaternion<T>::operator -=(const Quaternion<T>& q)
{
	w -= q.w;
	x -= q.x;
	y -= q.y;
	z -= q.z;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Quaternion<T>&
Quaternion<T>::operator *=(T scale)
{
	w *= scale;
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline const Quaternion<T>&
Quaternion<T>::operator /=(T scale)
{
	T inv = (T)1.0 / scale;
	w *= inv;
	x *= inv;
	y *= inv;
	z *= inv;
	return *this;
}
//---------------------------------------------------------------
template<typename T>
inline T*
Quaternion<T>::Ptr()
{
	return &w;
}
//---------------------------------------------------------------
template<typename T>
inline const T*
Quaternion<T>::Ptr() const
{
	return &w;
}
//---------------------------------------------------------------
template<typename T>
inline T
Quaternion<T>::Length()	const
{
	return Math<T>::Sqrt(w * w + x * x + y * y + z * z);
}
//---------------------------------------------------------------
template<typename T>
inline void
Quaternion<T>::Normalize()
{
	T invLen = 1.0f / Length();
	w *= invLen;
	x *= invLen;
	y *= invLen;
	z *= invLen;
}
//-------------------------------------------------------------------

// note the order of the rotation, the result of ql * qr is first rotate qr then rotate ql
template<typename T>
Quaternion<T> Quaternion<T>::operator * (const Quaternion<T>& r) const
{
	return Quaternion<T>(w * r.w - x * r.x - y * r.y - z * r.z,
		w * r.x + x * r.w + y * r.z - z * r.y,
		w * r.y + y * r.w + z * r.x - x * r.z,
		w * r.z + z * r.w + x * r.y - y * r.x);
}

template<typename U>
Vector3<U> operator * (const Vector3<U>& p, const Quaternion<U>& q)
{
	// q*P*q-1 = (s+v)P(s-v)
	//		   = s^2*P + 2s*v%P + (v.P)*v - v%P%v (% means cross product)
	U s = q.w;
	Vector3<U> v(q.x, q.y, q.z);
	Vector3<U> o = p * (s * s);
	o += Vector3<U>::Cross(v * ((U)2.0 * s), p);
	o += v * Vector3<U>::Dot(v, p);
	o -= Vector3<U>::Cross(Vector3<U>::Cross(v, p), v);

	return o;
}

template<typename U>
Vector3<U>&	operator *= (Vector3<U>& v, const Quaternion<U>& q)
{
	v = v * q;
	return v;
}

template<typename T>
Quaternion<T> Quaternion<T>::Inverse() const
{
	T inv = (T)1.0 / LengthSQ();
	return Quaternion<T>(w * inv, -x * inv, -y * inv, -z * inv);
}

template<typename T>
const Quaternion<T>& Quaternion<T>::InverseSelf()
{
	*this = Inverse();
	return *this;
}

template<typename T>
Matrix3<T>	Quaternion<T>::ToMat3() const
{
	T fTx = ((T)2.0) * x;
	T fTy = ((T)2.0) * y;
	T fTz = ((T)2.0) * z;
	T fTwx = fTx * w;
	T fTwy = fTy * w;
	T fTwz = fTz * w;
	T fTxx = fTx * x;
	T fTxy = fTy * x;
	T fTxz = fTz * x;
	T fTyy = fTy * y;
	T fTyz = fTz * y;
	T fTzz = fTz * z;

	return Matrix3<T>(
		(T)1.0 - (fTyy + fTzz), fTxy + fTwz, fTxz - fTwy,
		fTxy - fTwz, (T)1.0 - (fTxx + fTzz), fTyz + fTwx,
		fTxz + fTwy, fTyz - fTwx, (T)1.0 - (fTxx + fTyy));
}

template<typename T>
Matrix4<T>	Quaternion<T>::ToMat4() const
{
	T fTx = ((T)2.0) * x;
	T fTy = ((T)2.0) * y;
	T fTz = ((T)2.0) * z;
	T fTwx = fTx * w;
	T fTwy = fTy * w;
	T fTwz = fTz * w;
	T fTxx = fTx * x;
	T fTxy = fTy * x;
	T fTxz = fTz * x;
	T fTyy = fTy * y;
	T fTyz = fTz * y;
	T fTzz = fTz * z;

	return Matrix4<T>(
		(T)1.0 - (fTyy + fTzz), fTxy + fTwz, fTxz - fTwy, (T)0.0,
		fTxy - fTwz, (T)1.0 - (fTxx + fTzz), fTyz + fTwx, (T)0.0,
		fTxz + fTwy, fTyz - fTwx, (T)1.0 - (fTxx + fTyy), (T)0.0,
		(T)0.0, (T)0.0, (T)0.0, (T)1.0);
}


template<typename T>
T Quaternion<T>::Dot(const Quaternion<T>& a, const Quaternion<T>& b)
{
	return	a.w * b.w +
			a.x * b.x +
			a.y * b.y +
			a.z * b.z;
}

template<typename T>
Quaternion<T> Quaternion<T>::RotationAxis(const Vector3<T>& v, T angle)
{
	T halfAngle = ((T)0.5) * angle;
	T fSin = Math<T>::Sin(halfAngle);

	return Quaternion<T>(Math<T>::Cos(halfAngle), fSin * v.x, fSin * v.y, fSin * v.z);
}

template<typename T>
Quaternion<T> Quaternion<T>::FromVectorToVector(const Vector3<T>& vFrom, const Vector3<T>& vTo)
{
	Vector3<T> vFromNorm = vFrom;
	vFromNorm.Normalize();
	Vector3<T> vToNorm = vTo;
	vToNorm.Normalize();

	Vector3<T> vHalf = vFromNorm + vToNorm;
	vHalf.Normalize();

	Vector3<T> cross = Vector3f::Cross(vFromNorm, vHalf);
	T dot = Vector3f::Dot(vFromNorm, vHalf);
	return Quaternion<T>(dot, cross.x, cross.y, cross.z);
}

template<typename T>
Quaternion<T> Quaternion<T>::Slerp(const Quaternion<T>& ql, const Quaternion<T>& qr, T s)
{
	Quaternion<T>	r = qr;
	// range 1 to -1
	T cosOmega = Quaternion<T>::Dot(ql, qr);
	if (cosOmega < (T)0.0)
	{
		r *= (T)-1.0;
		cosOmega = -cosOmega;
	}

	T fCoef0, fCoef1;
	// very close just linear lerp
	if (cosOmega >(T)0.9999)
	{
		fCoef0 = (T)1.0 - s;
		fCoef1 = s;
	}
	else
	{
		// range 0 to 1
		T sinOmega = Math<T>::Sqrt((T)1.0 - cosOmega * cosOmega);
		// range 0 to pi
		T angle = Math<T>::ATan2(sinOmega, cosOmega);

		T fInvSin = (T)1.0 / sinOmega;
		fCoef0 = Math<T>::Sin(((T)1.0 - s)*angle) * fInvSin;
		fCoef1 = Math<T>::Sin(s * angle) * fInvSin;
	}

	return Quaternion<T>(fCoef0 * ql.w + fCoef1 * r.w,
		fCoef0 * ql.x + fCoef1 * r.x,
		fCoef0 * ql.y + fCoef1 * r.y,
		fCoef0 * ql.z + fCoef1 * r.z);
}

typedef Quaternion<float> Quaternionf;

inline Quaternionf ParseRotation(const std::string& str)
{
	float x, y, z, w;
	sscanf(str.c_str(), "%f,%f,%f,%f", &x, &y, &z, &w);
	return Quaternionf(w, x, y, z);
}

inline Quaternionf ParseRotation(const char* pStr)
{
	float x, y, z, w;
	sscanf(pStr, "%f,%f,%f,%f", &x, &y, &z, &w);
	return Quaternionf(w, x, y, z);
}


}//namespace




#endif