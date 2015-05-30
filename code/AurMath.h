#ifndef GAMATH_H
#define GAMATH_H

#include <cmath>
namespace Aurora
{
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











}//namespace




#endif