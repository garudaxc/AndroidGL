#ifndef GAMATHFUNCTION_H
#define GAMATHFUNCTION_H


#include "AurMath.h"

namespace FancyTech
{
//----------------------------------------------------------------------
// the order of combine between vector and matrix is form left to right
// that is (out = v * m)
template<typename real>
const Vector3<real>&
Vector3Transform(Vector3<real>& out, const Vector3<real>& v, const Matrix3<real>& mat)
{
	out.Set(v.x*mat._11 + v.y*mat._21 + v.z*mat._31,
			v.x*mat._12 + v.y*mat._22 + v.z*mat._32,
			v.x*mat._13 + v.y*mat._23 + v.z*mat._33);
	return out;
}
//----------------------------------------------------------------------
template<typename real>
void
Vector3TransformArray(const Vector3<real>* pIn, int iStride, const Vector3<real>* pOut,
					  int oStride, int count, const Matrix3<real>& mat)
{
	const char* pByteIn		= (const char*)pIn;
	      char* pByteOut	=		(char*)pOut;
	const Vector3<real>* pVecIn		= NULL;
		  Vector3<real>* pVecOut	= NULL;

	for (int i = 0; i < count; i++)
	{
		pVecIn	= (const Vector3<real>*)pByteIn;
		pVecOut =		(Vector3<real>*)pByteOut;

		pVecOut->Set(pVecIn->x*mat._11 + pVecIn->y*mat._21 + pVecIn->z*mat._31,
					pVecIn->x*mat._12 + pVecIn->y*mat._22 + pVecIn->z*mat._32,
					pVecIn->x*mat._13 + pVecIn->y*mat._23 + pVecIn->z*mat._33);

		pByteIn		+= iStride;
		pByteOut	+= oStride;
	}
	
}
//-----------------------------------------------------------------------
template<typename real>
const Vector3<real>&
Vector3Transform(Vector3<real>& out, const Vector3<real>& v, const Matrix4<real>& mat)
{
	out.Set(v.x*mat._11 + v.y*mat._21 + v.z*mat._31 + mat._41,
			v.x*mat._12 + v.y*mat._22 + v.z*mat._32 + mat._42,
			v.x*mat._13 + v.y*mat._23 + v.z*mat._33 + mat._43);
	return out;
}
//-----------------------------------------------------------------------
// rotate a vector with a 3 rows of a 4 row matrix
template<typename real>
const Vector3<real>&
Vector3Rotate(Vector3<real>& out, const Vector3<real>& v, const Matrix4<real>& mat)
{
	out.Set(v.x*mat._11 + v.y*mat._21 + v.z*mat._31,
			v.x*mat._12 + v.y*mat._22 + v.z*mat._32,
			v.x*mat._13 + v.y*mat._23 + v.z*mat._33);
	return out;
}
//======================
//
//======================
template<typename real>
const Vector3<real>&
Vector3Rotate(Vector3<real>& out, const Vector3<real>& p, const Quaternion<real>& q)
{
	// q*P*q-1 = (s+v)P(s-v)
	//		   = s^2*P + 2s*v%P + (v.P)*v - v%P%v (% means cross product)
	float s = q.w;
	Vector3<real> v(q.x, q.y, q.z);
	Vector3f o = p * (s * s);
	o += Vector3f::Cross(v * ((real)2.0 * s), p);
	o += v * Vector3f::Dot(v, p);
	o -= Vector3f::Cross(Vector3f::Cross(v, p), v);

	return out = o;
}
//----------------------------------------------------------------------
template<typename real>
void
Vector3TransformArray(const Vector3<real>* pIn, int iStride, Vector3<real>* pOut,
					  int oStride, int count, const Matrix4<real>& mat)
{
	const char* pByteIn		= (const char*)pIn;
	      char* pByteOut	=		(char*)pOut;
	const Vector3<real>* pVecIn		= 0;
		  Vector3<real>* pVecOut	= 0;

	for (int i = 0; i < count; i++)
	{
		pVecIn	= (const Vector3<real>*)pByteIn;
		pVecOut =		(Vector3<real>*)pByteOut;

		pVecOut->Set(pVecIn->x*mat._11 + pVecIn->y*mat._21 + pVecIn->z*mat._31 + mat._41,
					pVecIn->x*mat._12 + pVecIn->y*mat._22 + pVecIn->z*mat._32 + mat._42,
					pVecIn->x*mat._13 + pVecIn->y*mat._23 + pVecIn->z*mat._33 + mat._43);

		pByteIn		+= iStride;
		pByteOut	+= oStride;
	}
	
}
//------------------------------------------------------------------------
template<typename real>
const Vector3<real>&
Vector3Transform(Vector3<real>& out, const Matrix4<real>& matrix, const Vector3<real>& v)
{
	out.Set(v.x*matrix._11 + v.y*matrix._12 + v.z*matrix._13 + matrix._14,
			v.x*matrix._21 + v.y*matrix._22 + v.z*matrix._23 + matrix._24,
			v.x*matrix._31 + v.y*matrix._32 + v.z*matrix._33 + matrix._34);
	return out;
}
//-----------------------------------------------------------------------
template<typename real>
const Vector4<real>&
Vector4Transform(Vector4<real>& out, const Vector4<real>& v, const Matrix4<real>& mat)
{
	out.Set(v.x*mat._11 + v.y*mat._21 + v.z*mat._31 + v.w*mat._41,
			v.x*mat._12 + v.y*mat._22 + v.z*mat._32 + v.w*mat._42,
			v.x*mat._13 + v.y*mat._23 + v.z*mat._33 + v.w*mat._43,
			v.x*mat._14 + v.y*mat._24 + v.z*mat._34 + v.w*mat._44);
	return out;
}
//-----------------------------------------------------------------------
template<typename real>
const Vector2<real>&
VectorLerp(Vector2<real>& out, const Vector2<real>& v1, const Vector2<real>& v2, real s)
{
	out.Set(v1.x + s * (v2.x - v1.x),
			v1.y + s * (v2.y - v1.y));
	return out;
}
//------------------------------------------------------------------------
template<typename real>
const Vector3<real>&
VectorLerp(Vector3<real>& out, const Vector3<real>& v1, const Vector3<real>& v2, real s)
{
	out.Set(v1.x + s * (v2.x - v1.x),
			v1.y + s * (v2.y - v1.y),
			v1.z + s * (v2.z - v1.z));
	return out;
}
//------------------------------------------------------------------------
template<typename real>
const Matrix3<real>&
MatrixMultiply(Matrix3<real>& out, const Matrix3<real>& l, const Matrix3<real>& r)
{
	out.Set(
	/*out._11 = */l._11 * r._11 + l._12 * r._21 + l._13 * r._31,
	/*out._12 = */l._11 * r._12 + l._12 * r._22 + l._13 * r._32,
	/*out._13 = */l._11 * r._13 + l._12 * r._23 + l._13 * r._33,

	/*out._21 = */l._21 * r._11 + l._22 * r._21 + l._23 * r._31,
	/*out._22 = */l._21 * r._12 + l._22 * r._22 + l._23 * r._32,
	/*out._23 = */l._21 * r._13 + l._22 * r._23 + l._23 * r._33,

	/*out._31 = */l._31 * r._11 + l._32 * r._21 + l._33 * r._31,
	/*out._32 = */l._31 * r._12 + l._32 * r._22 + l._33 * r._32,
	/*out._33 = */l._31 * r._13 + l._32 * r._23 + l._33 * r._33);
	return out;
}

//-------------------------------------------------------------------
template<typename real>
const Matrix3<real>&
MatrixRotationX(Matrix3<real>& mat, real angle)
{
	mat = Matrix3<real>::IDENTITY;
	real spitch = Math<real>::Sin(angle);
	real cpitch = Math<real>::Cos(angle);

	mat._22 = cpitch;
	mat._23 = spitch;
	mat._32 = -spitch;
	mat._33 = cpitch;
	return mat;
}
//-------------------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixRotationX(Matrix4<real>& mat, real angle)
{
	mat = Matrix4<real>::IDENTITY;
	real spitch = Math<real>::Sin(angle);
	real cpitch = Math<real>::Cos(angle);

	mat._22 = cpitch;
	mat._23 = spitch;
	mat._32 = -spitch;
	mat._33 = cpitch;
	return mat;
}
//---------------------------------------------------------------------
template<typename real>
const Matrix3<real>&
MatrixRotationY(Matrix3<real>& mat, real angle)
{
	mat = Matrix3<real>::IDENTITY;
	real syaw = Math<real>::Sin(angle);
	real cyaw = Math<real>::Cos(angle);

	mat._11 = cyaw;
	mat._13 = -syaw;
	mat._31 = syaw;
	mat._33 = cyaw;
	return mat;
}

//---------------------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixRotationY(Matrix4<real>& mat, real angle)
{
	mat = Matrix4<real>::IDENTITY;
	real syaw = Math<real>::Sin(angle);
	real cyaw = Math<real>::Cos(angle);

	mat._11 = cyaw;
	mat._13 = -syaw;
	mat._31 = syaw;
	mat._33 = cyaw;
	return mat;
}
//-----------------------------------------------------------
template<typename real>
const Matrix3<real>&
MatrixRotationZ(Matrix3<real>& mat, real angle)
{
	mat = Matrix3<real>::IDENTITY;
	real sroll = Math<real>::Sin(angle);
	real croll = Math<real>::Cos(angle);

	mat._11 = croll;
	mat._12 = sroll;
	mat._21 = -sroll;
	mat._22 = croll;
	return mat;
}

//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixRotationZ(Matrix4<real>& mat, real angle)
{
	mat = Matrix4<real>::IDENTITY;
	real sroll = Math<real>::Sin(angle);
	real croll = Math<real>::Cos(angle);

	mat._11 = croll;
	mat._12 = sroll;
	mat._21 = -sroll;
	mat._22 = croll;
	return mat;
}
//-----------------------------------------------------------
template<typename real>
const Matrix3<real>&
MatrixScale(Matrix3<real>& mat, const Vector3<real>& s)
{
	mat = Matrix3<real>::IDENTITY;
	mat._11 = s.x;
	mat._22 = s.y;
	mat._33 = s.z;
	return mat;
}

//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixScale(Matrix4<real>& mat, const Vector3<real>& s)
{
	mat = Matrix4<real>::IDENTITY;
	mat._11 = s.x;
	mat._22 = s.y;
	mat._33 = s.z;
	return mat;
}
//-----------------------------------------------------------
template<typename real>
const Matrix3<real>&
MatrixRotationYawPitchRoll(Matrix3<real>& out, real yaw, real pitch, real roll)
{
	real sz = Math<real>::Sin(roll);
	real cz = Math<real>::Cos(roll);
	real sy = Math<real>::Sin(yaw);
	real cy = Math<real>::Cos(yaw);
	real sx = Math<real>::Sin(pitch);
	real cx = Math<real>::Cos(pitch);
	real sxsy = sx * sy;
	real sxcy = sx * cy;

	out._11 = cz * cy + sz * sxsy;		out._12 = sz * cx;		out._13 = -cz * sy + sz * sxcy;
	out._21 = -sz * cy + cz * sxsy;		out._22 = cz * cx;		out._23 = sz * sy + cz * sxcy;
	out._31 = cx * sy;					out._32 = -sx;			out._33 = cx * cy;
	return out;
}
//-----------------------------------------------------------
template<typename real>
const Matrix3<real>&
MatrixRotationFromMatrix(Matrix3<real>& out, const Matrix4<real>& matTrans)
{
	out.Set(
		matTrans._11, matTrans._12, matTrans._13,
		matTrans._21, matTrans._22, matTrans._23,
		matTrans._31, matTrans._32, matTrans._33);
	return out;
}
//-----------------------------------------------------------
template<typename real>
const Matrix3<real>&
MatrixRotationAxis(Matrix3<real>& out, const Vector3<real>& axis, real angle)
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

	out._11 = x2 * minusCos + c;		out._12 = xy * minusCos + v.z * s;	out._13 = xz * minusCos - v.y * s;
	out._21 = xy * minusCos - v.z * s;	out._22 = y2 * minusCos + c;		out._23 = yz * minusCos + v.x * s;
	out._31 = xz * minusCos + v.y * s;	out._32 = yz * minusCos - v.x * s;	out._33 = z2 * minusCos + c;
	return out;
}
//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixRotationAxis(Matrix4<real>& out, const Vector3<real>& axis, real angle)
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

	out._11 = x2 * minusCos + c;		out._12 = xy * minusCos + v.z * s;	out._13 = xz * minusCos - v.y * s;	out._14 = (real)0.0;
	out._21 = xy * minusCos - v.z * s;	out._22 = y2 * minusCos + c;		out._23 = yz * minusCos + v.x * s;	out._24 = (real)0.0;
	out._31 = xz * minusCos + v.y * s;	out._32 = yz * minusCos - v.x * s;	out._33 = z2 * minusCos + c;		out._34 = (real)0.0;
	out._41 = (real)0.0;				out._42 = (real)0.0;				out._43 = (real)0.0;				out._44 = (real)1.0;
	return out;
}

//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixEulerXYZ(Matrix4<real>& out, real x, real y, real z)
{
	real sx = Math<real>::Sin(x);
	real cx = Math<real>::Cos(x);
	real sy = Math<real>::Sin(y);
	real cy = Math<real>::Cos(y);
	real sz = Math<real>::Sin(z);
	real cz = Math<real>::Cos(z);

	out.Set(cy * cz,					cy * sz,					-sy,		(real)0.0,
			sy * sx * cz - cx * sz,		sy * sx * sz + cx * cz,		cy * sx,	(real)0.0,
			sy * cx * cz + sx * sz,		sy * cx * sz - sx * cz,		cy * cx,	(real)0.0,
			(real)0.0,					(real)0.0,					(real)0.0,	(real)1.0);
	return out;
}
//-----------------------------------------------------------
// calculate rotation matrix form quaternion
template<typename real>
const Matrix4<real>&
MatrixFromQuaternion(Matrix4<real>& mat, const Quaternion<real>& q)
{
	real fTx  = ((real)2.0)*q.x;
	real fTy  = ((real)2.0)*q.y;
	real fTz  = ((real)2.0)*q.z;
	real fTwx = fTx * q.w;
	real fTwy = fTy * q.w;
	real fTwz = fTz * q.w;
	real fTxx = fTx * q.x;
	real fTxy = fTy * q.x;
	real fTxz = fTz * q.x;
	real fTyy = fTy * q.y;
	real fTyz = fTz * q.y;
	real fTzz = fTz * q.z;

	mat._11 = (real)1.0 - (fTyy + fTzz);
	mat._12 = fTxy + fTwz;
	mat._13 = fTxz - fTwy;
	mat._14 = (real)0.0;
	mat._21 = fTxy - fTwz;
	mat._22 = (real)1.0 - (fTxx + fTzz);
	mat._23 = fTyz + fTwx;
	mat._24 = (real)0.0;
	mat._31 = fTxz + fTwy;
	mat._32 = fTyz - fTwx;
	mat._33 = (real)1.0 - (fTxx + fTyy);
	mat._34 = (real)0.0;
	mat._41 = (real)0.0;
	mat._42 = (real)0.0;
	mat._43 = (real)0.0;
	mat._44 = (real)1.0;
	return mat;
}

//-----------------------------------------------------------
// calculate rotation matrix form quaternion
template<typename real>
const Matrix3<real>&
MatrixFromQuaternion(Matrix3<real>& mat, const Quaternion<real>& q)
{
	real fTx  = ((real)2.0)*q.x;
	real fTy  = ((real)2.0)*q.y;
	real fTz  = ((real)2.0)*q.z;
	real fTwx = fTx * q.w;
	real fTwy = fTy * q.w;
	real fTwz = fTz * q.w;
	real fTxx = fTx * q.x;
	real fTxy = fTy * q.x;
	real fTxz = fTz * q.x;
	real fTyy = fTy * q.y;
	real fTyz = fTz * q.y;
	real fTzz = fTz * q.z;

	mat._11 = (real)1.0 - (fTyy + fTzz);
	mat._12 = fTxy + fTwz;
	mat._13 = fTxz - fTwy;
	mat._21 = fTxy - fTwz;
	mat._22 = (real)1.0 - (fTxx + fTzz);
	mat._23 = fTyz + fTwx;
	mat._31 = fTxz + fTwy;
	mat._32 = fTyz - fTwx;
	mat._33 = (real)1.0 - (fTxx + fTyy);
	return mat;
}
//-----------------------------------------------------------
// compute determinant of a 3x3 matrix
template<typename real>
real
MatrixDeterminant(const Matrix3<real>& mat)
{
	real fC11 = mat._22 * mat._33 - mat._23 * mat._32;
	real fC12 = mat._23 * mat._31 - mat._21 * mat._33;
	real fC13 = mat._21 * mat._32 - mat._22 * mat._31;

	return mat._11 * fC11 + mat._12 * fC12 + mat._13 * fC13;
}
//-----------------------------------------------------------
// get inverse of a matrix
template<typename real>
const Matrix3<real>&
MatrixInverse(Matrix3<real>& out, const Matrix3<real>& mat)
{
	real fC11 = mat._22 * mat._33 - mat._23 * mat._32;
	real fC12 = mat._23 * mat._31 - mat._21 * mat._33;
	real fC13 = mat._21 * mat._32 - mat._22 * mat._31;

	real det =  mat._11 * fC11 + mat._12 * fC12 + mat._13 * fC13;

	// the matrix has inverse
	if (Math<real>::Abs(det) > Math<real>::ZERO_TOLERANCE)
	{
		out._11 = fC11;
        out._12 = mat._13 * mat._32 - mat._12 * mat._33;
        out._13 = mat._12 * mat._23 - mat._13 * mat._22;
        out._21 = fC12;
        out._22 = mat._11 * mat._33 - mat._13 * mat._31;
        out._23 = mat._13 * mat._21 - mat._11 * mat._23;
        out._31 = fC13;
        out._32 = mat._12 * mat._31 - mat._11 * mat._32;
        out._33 = mat._11 * mat._22 - mat._12 * mat._21;

		real inv = (real)1.0 / det;
		out._11 *= inv;
		out._12 *= inv;
		out._13 *= inv;
		out._21 *= inv;
		out._22 *= inv;
		out._23 *= inv;
		out._31 *= inv;
		out._32 *= inv;
		out._33 *= inv;
	}
	// the matrix dosen't has inverse
	else
	{
		out = Matrix3<real>::ZERO;
	}
	return out;
}
//-----------------------------------------------------------
template<typename real>
const Matrix3<real>&
MatrixMultiplyDiagonal(Matrix3<real>& out, const Vector3<real>& v, const Matrix3<real>& mat)
{
	 out.Set(v.x * mat._11, v.x * mat._12, v.x * mat._13, 
			v.y * mat._21, v.y * mat._22, v.y * mat._23, 
			v.z * mat._31, v.z * mat._32, v.z * mat._33);
	 return out;
}

//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixMultiply(Matrix4<real>& out, const Matrix4<real>& l, const Matrix4<real>& r)
{
	out.Set(
	/*out._11 = */l._11 * r._11 + l._12 * r._21 + l._13 * r._31 + l._14 * r._41,
	/*out._12 = */l._11 * r._12 + l._12 * r._22 + l._13 * r._32 + l._14 * r._42,
	/*out._13 = */l._11 * r._13 + l._12 * r._23 + l._13 * r._33 + l._14 * r._43,
	/*out._14 = */l._11 * r._14 + l._12 * r._24 + l._13 * r._34 + l._14 * r._44,

	/*out._21 = */l._21 * r._11 + l._22 * r._21 + l._23 * r._31 + l._24 * r._41,
	/*out._22 = */l._21 * r._12 + l._22 * r._22 + l._23 * r._32 + l._24 * r._42,
	/*out._23 = */l._21 * r._13 + l._22 * r._23 + l._23 * r._33 + l._24 * r._43,
	/*out._24 = */l._21 * r._14 + l._22 * r._24 + l._23 * r._34 + l._24 * r._44,

	/*out._31 = */l._31 * r._11 + l._32 * r._21 + l._33 * r._31 + l._34 * r._41,
	/*out._32 = */l._31 * r._12 + l._32 * r._22 + l._33 * r._32 + l._34 * r._42,
	/*out._33 = */l._31 * r._13 + l._32 * r._23 + l._33 * r._33 + l._34 * r._43,
	/*out._34 = */l._31 * r._14 + l._32 * r._24 + l._33 * r._34 + l._34 * r._44,

	/*out._41 = */l._41 * r._11 + l._42 * r._21 + l._43 * r._31 + l._44 * r._41,
	/*out._42 = */l._41 * r._12 + l._42 * r._22 + l._43 * r._32 + l._44 * r._42,
	/*out._43 = */l._41 * r._13 + l._42 * r._23 + l._43 * r._33 + l._44 * r._43,
	/*out._44 = */l._41 * r._14 + l._42 * r._24 + l._43 * r._34 + l._44 * r._44);
	return out;
}
//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixTranslation(Matrix4<real>& mat, real x, real y, real z)
{
	mat = Matrix4<real>::IDENTITY;
	mat._41 = x;
	mat._42 = y;
	mat._43 = z;
	return mat;
}
//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixTranslation(Matrix4<real>& mat, const Vector3<real>& vec)
{
	mat = Matrix4<real>::IDENTITY;
	mat._41 = vec.x;
	mat._42 = vec.y;
	mat._43 = vec.z;
	return mat;
}
//-----------------------------------------------------------
// get inverse of a matrix
template<typename real>
const Matrix4<real>&
MatrixInverse(Matrix4<real>& out, const Matrix4<real>& mat)
{
	real fA0 = mat.m[ 0]*mat.m[ 5] - mat.m[ 1]*mat.m[ 4];
    real fA1 = mat.m[ 0]*mat.m[ 6] - mat.m[ 2]*mat.m[ 4];
    real fA2 = mat.m[ 0]*mat.m[ 7] - mat.m[ 3]*mat.m[ 4];
    real fA3 = mat.m[ 1]*mat.m[ 6] - mat.m[ 2]*mat.m[ 5];
    real fA4 = mat.m[ 1]*mat.m[ 7] - mat.m[ 3]*mat.m[ 5];
    real fA5 = mat.m[ 2]*mat.m[ 7] - mat.m[ 3]*mat.m[ 6];
    real fB0 = mat.m[ 8]*mat.m[13] - mat.m[ 9]*mat.m[12];
    real fB1 = mat.m[ 8]*mat.m[14] - mat.m[10]*mat.m[12];
    real fB2 = mat.m[ 8]*mat.m[15] - mat.m[11]*mat.m[12];
    real fB3 = mat.m[ 9]*mat.m[14] - mat.m[10]*mat.m[13];
    real fB4 = mat.m[ 9]*mat.m[15] - mat.m[11]*mat.m[13];
    real fB5 = mat.m[10]*mat.m[15] - mat.m[11]*mat.m[14];

    real fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
	if (Math<real>::Abs(fDet) <= Math<real>::ZERO_TOLERANCE)
	{
		return Matrix4<real>::ZERO;
	}

	out.Set(
        + mat.m[ 5]*fB5 - mat.m[ 6]*fB4 + mat.m[ 7]*fB3,
        - mat.m[ 1]*fB5 + mat.m[ 2]*fB4 - mat.m[ 3]*fB3,
        + mat.m[13]*fA5 - mat.m[14]*fA4 + mat.m[15]*fA3,
        - mat.m[ 9]*fA5 + mat.m[10]*fA4 - mat.m[11]*fA3,
        - mat.m[ 4]*fB5 + mat.m[ 6]*fB2 - mat.m[ 7]*fB1,
        + mat.m[ 0]*fB5 - mat.m[ 2]*fB2 + mat.m[ 3]*fB1,
        - mat.m[12]*fA5 + mat.m[14]*fA2 - mat.m[15]*fA1,
        + mat.m[ 8]*fA5 - mat.m[10]*fA2 + mat.m[11]*fA1,
        + mat.m[ 4]*fB4 - mat.m[ 5]*fB2 + mat.m[ 7]*fB0,
        - mat.m[ 0]*fB4 + mat.m[ 1]*fB2 - mat.m[ 3]*fB0,
        + mat.m[12]*fA4 - mat.m[13]*fA2 + mat.m[15]*fA0,
        - mat.m[ 8]*fA4 + mat.m[ 9]*fA2 - mat.m[11]*fA0,
        - mat.m[ 4]*fB3 + mat.m[ 5]*fB1 - mat.m[ 6]*fB0,
        + mat.m[ 0]*fB3 - mat.m[ 1]*fB1 + mat.m[ 2]*fB0,
        - mat.m[12]*fA3 + mat.m[13]*fA1 - mat.m[14]*fA0,
        + mat.m[ 8]*fA3 - mat.m[ 9]*fA1 + mat.m[10]*fA0);

//	out /= fDet;
	return out;
}
//-----------------------------------------------------------
//-----------------------------------------------------------
//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixLookAtLH(Matrix4<real>& matrix, const Vector3<real>& vEye,
			   const Vector3<real>& vAt = Vector3<real>(0.0f, 0.0f, 0.0f), 
			   const Vector3<real>& vUp = Vector3<real>(0.0f, 1.0f, 0.0f))
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
	matrix.Set(x.x, y.x, z.x, 0.0f,
			   x.y, y.y, z.y, 0.0f,
			   x.z, y.z, z.z, 0.0f,
			   -Vector3f::Dot(x, vEye), -Vector3f::Dot(y, vEye), -Vector3f::Dot(z, vEye), 1.0f);
	return matrix;
}
//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixLookAtRH(Matrix4<real>& matrix, const Vector3<real>& vEye,
			   const Vector3<real>& vAt = Vector3<real>(0.0f, 0.0f, 0.0f), 
			   const Vector3<real>& vUp = Vector3<real>(0.0f, 1.0f, 0.0f))
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
	matrix.Set(x.x, y.x, z.x, 0.0f,
			   x.y, y.y, z.y, 0.0f,
			   x.z, y.z, z.z, 0.0f,
			   -Vector3f::Dot(x, vEye), -Vector3f::Dot(y, vEye), -Vector3f::Dot(z, vEye), 1.0f);
	return matrix;
}
//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixPerspectiveFovLH(Matrix4<real>& matrix, real fovy = Math<real>::PI / 4.0f,
					   real aspect = 1.0f, real zn = 1.0f, real zf = 1000.0f)
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

	matrix.Set(xScale, 0.0f, 0.0f, 0.0f,
			   0.0f, yScale, 0.0f, 0.0f,
			   0.0f, 0.0f, zScale, 1.0f,
			   0.0f, 0.0f, -zn * zScale, 0.0f);
	return matrix;
}
//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixPerspectiveFovRH(Matrix4<real>& matrix, real fovy = Math<real>::PI / 4.0f,
					   real aspect = 1.0f, real zn = 1.0f, real zf = 1000.0f)
{


	real yScale = 1.0f / Math<real>::Tan(fovy / 2.0f);
	real xScale = yScale / aspect;
	real zScale = zf / (zn - zf);

	matrix.Set(xScale, 0.0f, 0.0f, 0.0f,
			   0.0f, yScale, 0.0f, 0.0f,
			   0.0f, 0.0f, zScale, -1.0f,
			   0.0f, 0.0f, zn * zScale, 0.0f);
	return matrix;
}
//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixOrthoLH(Matrix4<real>& matrix, real width, real height, real zn = 1.0f, real zf = 1000.0f)
{
	real InvDepth = 1.0f / (zf - zn);

	matrix.Set(2.0f / width, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / height, 0.0f, 0.0f,
		0.0f, 0.0f, InvDepth, 0.0f,
		0.0f, 0.0f, -zn * InvDepth, 1.0f);
	return matrix;
}
//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixOrthoRH(Matrix4<real>& matrix, real width, real height, real zn = 1.0f, real zf = 1000.0f)
{
	real InvDepth = 1.0f / (zn - zf);

	matrix.Set(2.0f / width, 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f / height, 0.0f, 0.0f,
				0.0f, 0.0f, InvDepth, 0.0f,
				0.0f, 0.0f, zn * InvDepth, 1.0f);
	return matrix;
}
//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixOrthoOffCenterLH(Matrix4<real>& matrix, real l,  real r, real b, real t,
										real zn = 1.0f, real zf = 1000.0f)
{
	real InvDepth = 1.0f / (zf - zn);

	matrix.Set(2.0f / (r-l), 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f / (t-b), 0.0f, 0.0f,
				0.0f, 0.0f, InvDepth, 0.0f,
				(l+r)/(l-r), (t+b)/(b-t), -zn * InvDepth, 1.0f);
	return matrix;
}
//-----------------------------------------------------------
template<typename real>
const Matrix4<real>&
MatrixOrthoOffCenterRH(Matrix4<real>& matrix, real l,  real r, real b, real t,
					   real zn = 1.0f, real zf = 1000.0f)
{
	real InvDepth = 1.0f / (zn - zf);

	matrix.Set(2.0f / (r-l), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (t-b), 0.0f, 0.0f,
			0.0f, 0.0f, InvDepth, 0.0f,
			(l+r)/(l-r), (t+b)/(b-t), zn * InvDepth, 1.0f);
	return matrix;
}
//-----------------------------------------------------------
// calculate quaternion from rotation axis and angle
// v should be normalized
template<typename real>
const Quaternion<real>&
QuaternionRotationAxis(Quaternion<real>& q, const Vector3<real>& v, real angle)
{
	real halfAngle = ((real)0.5) * angle;
    real fSin = Math<real>::Sin(halfAngle);
    q.w = Math<real>::Cos(halfAngle);
    q.x = fSin * v.x;
    q.y = fSin * v.y;
    q.z = fSin * v.z;
	return q;
}

//-----------------------------------------------------------
// calculate rotation matrix form quaternion
template<typename real>
Quaternion<real>&
QuaternionFromRotationMatrix(Quaternion<real>& out, const Matrix4<real>& matrix)
{
	const real* mat = matrix.Ptr();

	real T = mat[0] + mat[5] + mat[10];

	if (T > (real)0.0)
	{
		real S = Math<real>::Sqrt((real)1.0 + T) * (real)2;
		out.x = ( mat[6] - mat[9] ) / S;
		out.y = ( mat[8] - mat[2] ) / S;
		out.z = ( mat[1] - mat[4] ) / S;
		out.w = (real)0.25 * S;
	}
	else if ( mat[0] > mat[5] && mat[0] > mat[10] )			// Column 0: 
	{	
		real S  = Math<real>::Sqrt( (real)1.0 + mat[0] - mat[5] - mat[10] ) * (real)2;
		out.x = (real)0.25 * S;
		out.y = (mat[4] + mat[1] ) / S;
		out.z = (mat[2] + mat[8] ) / S;
		out.w = (mat[6] - mat[9] ) / S;
	}
	else if ( mat[5] > mat[10] )						// Column 1: 
	{			
		real S  = Math<real>::Sqrt( (real)1.0 + mat[5] - mat[0] - mat[10] ) * (real)2;
		out.x = (mat[4] + mat[1] ) / S;
		out.y = (real)0.25 * S;
		out.z = (mat[9] + mat[6] ) / S;
		out.w = (mat[8] - mat[2] ) / S;
	}
	else												// Column 2:
	{						
		real S  = Math<real>::Sqrt( (real)1.0 + mat[10] - mat[0] - mat[5] ) * (real)2;
		out.x = (mat[2] + mat[8] ) / S;
		out.y = (mat[9] + mat[6] ) / S;
		out.z = (real)0.25 * S;
		out.w = (mat[1] - mat[4] ) / S;
	}

	return out;
}
//-----------------------------------------------------------
template<typename real>
const Quaternion<real>&
QuaternionConjugate(Quaternion<real>& out, const Quaternion<real>& q)
{
	out.Set(q.w, -q.x, -q.y, -q.z);
	return out;
}

//-----------------------------------------------------------
template<typename real>
const Quaternion<real>&
QuaternionInverse(Quaternion<real>& out, const Quaternion<real>& q)
{
	real inv = (real)1.0 / q.LengthSQ();
	out.Set(q.w * inv, -q.x * inv, -q.y * inv, -q.z * inv);
	return out;
}
////-----------------------------------------------------------
//template<typename real>
//const Quaternion<real>&
//QuaternionRotationYawPitchRoll(Quaternion<real>& q, real yaw, real pitch, real roll)
//{
//	return q;
//}
////-----------------------------------------------------------
//template<typename real>
//const Quaternion<real>&
//QuaternionEulerXYZ(Quaternion<real>& q, real X, real Y, real Z)
//{
//	// Assuming the angles are in radians.   
//	real c1 = Math<real>::Cos(X / (real)2.0);    
//	real s1 = Math<real>::Sin(X / (real)2.0);   
//	real c2 = Math<real>::Cos(Y / (real)2.0);   
//	real s2 = Math<real>::Sin(Y / (real)2.0);    
//	real c3 = Math<real>::Cos(Z / (real)2.0);   
//	real s3 = Math<real>::Sin(Z / (real)2.0);
//	real c1c2 = c1 * c2;   
//	real s1s2 = s1 * s2;   
//	real w =c1c2 * c3 - s1s2 * s3;
//	real x =c1c2 * s3 + s1s2 * c3;
//	real y =s1 * c2 * c3 + c1 * s2 * s3;
//	real z =c1 * s2 * c3 - s1 * c2 * s3;
//	q.Set(w, x, y, z);
//	return q;
//}
//-----------------------------------------------------------
// note the order of the rotation, the result of ql * qr is first rotate qr then rotate ql
template<typename real>
const Quaternion<real>&
QuaternionMultiply(Quaternion<real>& out, const Quaternion<real>& l, const Quaternion<real>& r)
{
	out.Set(l.w * r.w - l.x * r.x - l.y * r.y - l.z * r.z,
			l.w * r.x + l.x * r.w + l.y * r.z - l.z * r.y,
			l.w * r.y + l.y * r.w + l.z * r.x - l.x * r.z,
			l.w * r.z + l.z * r.w + l.x * r.y - l.y * r.x);
	return out;
}
//-----------------------------------------------------------
// get a rotation from vector to vector
template<typename real>
const Quaternion<real>&
QuaternionVectorToVector(Quaternion<real>& out, const Vector3<real>& vFrom, const Vector3<real>& vTo)
{
	Vector3<real> vFromNorm = vFrom;
	vFromNorm.Normalize();
	Vector3<real> vToNorm = vTo;
	vToNorm.Normalize();

	Vector3<real> vHalf = vFromNorm + vToNorm;
	vHalf.Normalize();

	Vector3<real> cross = Vector3f::Cross(vFromNorm, vHalf);
	real dot = Vector3f::Dot(vFromNorm, vHalf);
	out.Set(dot, cross.x, cross.y, cross.z);
	return out;	
}
//-----------------------------------------------------------
template<typename real>
const Quaternion<real>&
QuaternionSlerp(Quaternion<real>& out, const Quaternion<real>& ql, const Quaternion<real>& qr, real s)
{
	Quaternion<real>	r = qr;
	// range 1 to -1
	real cosOmega = Vector3f::Dot(ql, qr);
	if (cosOmega < (real)0.0)
	{
		r *= (real)-1.0;
		cosOmega = -cosOmega;
	}

	real fCoef0, fCoef1;
	// very close just linear lerp
	if (cosOmega > (real)0.9999)
	{
		fCoef0 = (real)1.0 - s;
		fCoef1 = s;
	}
	else
	{
		// range 0 to 1
		real sinOmega = Math<real>::Sqrt((real)1.0 - cosOmega * cosOmega);
		// range 0 to pi
		real angle = Math<real>::ATan2(sinOmega, cosOmega);

		real fInvSin = (real)1.0 / sinOmega;
		fCoef0 = Math<real>::Sin(((real)1.0 - s)*angle) * fInvSin;
		fCoef1 = Math<real>::Sin(s * angle) * fInvSin;
	}

	out.Set(fCoef0 * ql.w + fCoef1 * r.w,
			fCoef0 * ql.x + fCoef1 * r.x,
			fCoef0 * ql.y + fCoef1 * r.y,
			fCoef0 * ql.z + fCoef1 * r.z);
	return out;
}
//-----------------------------------------------------------
// normalized linear lerp for quaternion , as we do not always need const velocity rotation interpolation
// normalized linear lerp will be easy and efficient
template<typename real>
const Quaternion<real>&
QuaternionNlerp(Quaternion<real>& out, const Quaternion<real>& ql, const Quaternion<real>& qr, real s)
{
	out.Set(ql.w + s * (qr.w - ql.w),
		ql.x + s * (qr.x - ql.x),
		ql.y + s * (qr.y - ql.y),
		ql.z + s * (qr.z - ql.z));

	out.Normalize();
	return out;
}
//-----------------------------------------------------------
// make a 4x4 homogeneous matrix from a 3x3 rotation matrix and a 3x1 translation vector
// the order is rotation first, then translate
template<typename real>
void
MatrixTransform(Matrix4<real>& matTransform, const Matrix3<real>& matRot, const Vector3<real>& vecTrans)
{
	matTransform._11 = matRot._11;	matTransform._12 = matRot._12;	matTransform._13 = matRot._13;	matTransform._14 = (real)0.0;
	matTransform._21 = matRot._21;	matTransform._22 = matRot._22;	matTransform._23 = matRot._23;	matTransform._24 = (real)0.0;
	matTransform._31 = matRot._31;	matTransform._32 = matRot._32;	matTransform._33 = matRot._33;	matTransform._34 = (real)0.0;

	matTransform._41 = vecTrans.x;	matTransform._42 = vecTrans.y;	matTransform._43 = vecTrans.z;	matTransform._44 = (real)1.0;
	/*matTransform._41 = vecTrans.x * matRot._11 +  vecTrans.y * matRot._21 +  vecTrans.z * matRot._31;
	matTransform._42 = vecTrans.x * matRot._12 +  vecTrans.y * matRot._22 +  vecTrans.z * matRot._32;
	matTransform._43 = vecTrans.x * matRot._13 +  vecTrans.y * matRot._23 +  vecTrans.z * matRot._33;*/
}
//-----------------------------------------------------------
template<typename real>
void
MatrixTransform(Matrix4<real>& matTransform, const Quaternion<real>& qRot, const Vector3<real>& vecTrans)
{
	Matrix3<real>	matRot;
	MatrixFromQuaternion(matRot, qRot);

	matTransform._11 = matRot._11;	matTransform._12 = matRot._12;	matTransform._13 = matRot._13;	matTransform._14 = (real)0.0;
	matTransform._21 = matRot._21;	matTransform._22 = matRot._22;	matTransform._23 = matRot._23;	matTransform._24 = (real)0.0;
	matTransform._31 = matRot._31;	matTransform._32 = matRot._32;	matTransform._33 = matRot._33;	matTransform._34 = (real)0.0;

	matTransform._41 = vecTrans.x;	matTransform._42 = vecTrans.y;	matTransform._43 = vecTrans.z;	matTransform._44 = (real)1.0;
}



}// end of namespace
#endif