// Matrix3.h: interface for the Matrix3 class.
//
//////////////////////////////////////////////////////////////////////
#ifndef GAMATRIX3_H
#define GAMATRIX3_H

#include <memory.h>
#pragma warning( disable : 4201 )

namespace Aurora
{
//=================================================
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

	inline real*		Ptr();
	inline const real*	Ptr() const;
	inline real* GetRow(size_t row) const;

	inline void TransposeSelf();


	// special matrices
    static const Matrix3 ZERO;
    static const Matrix3 IDENTITY;

};
//==========================================================








//===========================================================
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
Matrix3<real>::Matrix3(const real* pData)
{
	memcpy(m, pData, sizeof(m));
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
	return &m[row*3];
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

typedef Matrix3<float> Matrix3f;



































//=================================================
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

	// constr
	Matrix4() {}
	Matrix4(const Matrix4<real>& matrix);
	Matrix4(const real* pData);
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

	inline real* Ptr();
	inline const real* Ptr() const;
	inline real* GetRow(size_t row) const;

	inline void TransposeSelf();

	// special matrices
    static const Matrix4 ZERO;
    static const Matrix4 IDENTITY;


};

//-----------------------------------------------------------------
template<typename real>
Matrix4<real>::Matrix4(const Matrix4<real>& matrix)
{
	memcpy(m, matrix.m, sizeof(m));
}

//-----------------------------------------------------------------
template<typename real>
Matrix4<real>::Matrix4(const real* pData)
{
	memcpy(m, pData, sizeof(m));
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
	_11 + m._11,	_12 + m._12,	_13 + m._13,	_14 + m._14,
	_21 + m._21,	_22 + m._22,	_23 + m._23,	_24 + m._24,
	_31 + m._31,	_32 + m._32,	_33 + m._33,	_34 + m._34,
	_41 + m._41,	_42 + m._42,	_43 + m._43,	_44 + m._44);

}
//-----------------------------------------------------------------
template<typename real>
inline Matrix4<real> 
Matrix4<real>::operator -(const Matrix4<real>& m) const
{
	return Matrix4<real>(
	_11 - m._11,	_12 - m._12,	_13 - m._13,	_14 - m._14,
	_21 - m._21,	_22 - m._22,	_23 - m._23,	_24 - m._24,
	_31 - m._31,	_32 - m._32,	_33 - m._33,	_34 - m._34,
	_41 - m._41,	_42 - m._42,	_43 - m._43,	_44 - m._44);
}
//-----------------------------------------------------------------
template<typename real>
inline Matrix4<real> 
Matrix4<real>::operator *(real scale) const
{
	return Matrix4<real>(
	_11 * scale,	_12 * scale,	_13 * scale,	_14 * scale,
	_21 * scale,	_22 * scale,	_23 * scale,	_24 * scale,
	_31 * scale,	_32 * scale,	_33 * scale,	_34 * scale,
	_41 * scale,	_42 * scale,	_43 * scale,	_44 * scale);
}
//-----------------------------------------------------------------
template<typename real>
inline Matrix4<real> 
Matrix4<real>::operator /(real scale) const
{
	real inv = (real)1.0 / scale;
	return Matrix4<real>(
	_11 * inv,	_12 * inv,	_13 * inv,	_14 * inv,
	_21 * inv,	_22 * inv,	_23 * inv,	_24 * inv,
	_31 * inv,	_32 * inv,	_33 * inv,	_34 * inv,
	_41 * inv,	_42 * inv,	_43 * inv,	_44 * inv);
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
	return &m[row*4];
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


typedef Matrix4<float> Matrix4f;

}

#endif
