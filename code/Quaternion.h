#ifndef GAQUATERNION_H
#define GAQUATERNION_H

#include <string>

namespace Aurora
{

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

	inline T*		Ptr();
	inline const T* Ptr() const;

	inline T LengthSQ()	const;
	inline T Length()	const;

	inline void Normalize();

	float w, x, y, z;

	
	static const Quaternion IDENTITY;

};



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
	return Quaternion<T>(w+q.w, x+q.x, y+q.y, z+q.z);
}
//---------------------------------------------------------------
template<typename T>
inline Quaternion<T> 
Quaternion<T>::operator -(const Quaternion<T>& q) const
{
	return Quaternion<T>(w-q.w, x-q.x, y-q.y, z-q.z);
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

template<typename T>
inline T
DotProduct(const Quaternion<T>& ql, const Quaternion<T>& qr)
{
	return ql.w * qr.w + 
		ql.x * qr.x + 
		ql.y * qr.y + 
		ql.z * qr.z;
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
}



#endif