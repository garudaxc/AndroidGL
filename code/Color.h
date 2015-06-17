#ifndef GACOLOR_H
#define GACOLOR_H

#include <string>
#include "AurMath.h"

namespace Aurora
{

class Color
{
public:
	// dummy constructor
	Color()	{}
	Color(const Color& c):r(c.r),g(c.g),b(c.b),a(c.a)	{}
	Color(float fr, float fg, float fb):r(fr),g(fg),b(fb),a(1.0f)	{}
	Color(float fr, float fg, float fb, float fa):r(fr),g(fg),b(fb),a(fa)	{}

	inline const Color& operator = (const Color& c);

	inline void Set(float fr, float fg, float fb, float fa);

	inline Color operator +(const Color& c) const;
	inline Color operator -(const Color& c) const;
	inline Color operator *(float scale) const;
	inline Color operator /(float scale) const;

	inline const Color& operator +=(const Color& c);
	inline const Color& operator -=(const Color& c);
	inline const Color& operator *=(float scale);
	inline const Color& operator /=(float scale);

	//clamp the color value between 0.0 and 1.0
	inline void EnValidate();
	inline float* Ptr();
	inline const float* Ptr() const;

	std::string			ToStringRGB() const;
	const Color&		FromStringRGB(const std::string& str);
	const Color&		FromStringRGBA(const std::string& str);

	float r, g, b, a;

	static const Color WHITE;
	static const Color BLACK;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color GRAY;
	static const Color ZERO;
};


inline void ColorModulate(Color& out, const Color& c1, const Color& c2);
inline const Color& ColorLerp(Color& out, const Color& c1, const Color& c2, float fLerp);

//=============================================================================


//-----------------------------------------------------------
inline const Color& 
Color::operator = (const Color& c)
{
	r = c.r;	g = c.g;	b = c.b;	a = c.a;
	return *this;
}
//-----------------------------------------------------------
inline void 
Color::Set(float fr, float fg, float fb, float fa)
{
	r = fr;	g = fg;	b = fb;	a = fa;
}
//-----------------------------------------------------------
inline Color 
Color::operator +(const Color& c) const
{
	return Color(r + c.r, g + c.g, b + c.b, a + c.a);
}
//-----------------------------------------------------------
inline Color 
Color::operator -(const Color& c) const
{
	return Color(r - c.r, g - c.g, b - c.b, a - c.a);
}
//-----------------------------------------------------------
inline Color 
Color::operator *(float scale) const
{
	return Color(r * scale, g * scale, b * scale, a * scale);
}
//-----------------------------------------------------------
inline Color 
Color::operator /(float scale) const
{
	float reciprocal = 1.0f / scale;
	return Color(r * reciprocal, g * reciprocal, b * reciprocal, a * reciprocal);
}
//-----------------------------------------------------------
inline const Color& 
Color::operator +=(const Color& c)
{
	r += c.r;
	g += c.g;
	b += c.b;
	a += c.a;
	return *this;
}
//-----------------------------------------------------------
inline const Color& 
Color::operator -=(const Color& c)
{
	r -= c.r;
	g -= c.g;
	b -= c.b;
	a -= c.a;
	return *this;
}
//-----------------------------------------------------------
inline const Color& 
Color::operator *=(float scale)
{
	r *= scale;
	g *= scale;
	b *= scale;
	a *= scale;
	return *this;
}
//-----------------------------------------------------------
inline const Color& Color::operator /=(float scale)
{
	float reciprocal = 1.0f / scale;
	r *= reciprocal;
	g *= reciprocal;
	b *= reciprocal;
	a *= reciprocal;
	return *this;
}
//-----------------------------------------------------------
inline void Color::EnValidate()
{
	r = Mathf::Clamp(r, 0.0f, 1.0f);
	g = Mathf::Clamp(g, 0.0f, 1.0f);
	b = Mathf::Clamp(b, 0.0f, 1.0f);
	a = Mathf::Clamp(a, 0.0f, 1.0f);
}
//-----------------------------------------------------------
inline float*
Color::Ptr()
{
	return &r;
}
//-----------------------------------------------------------
inline const float* Color::Ptr() const
{
	return &r;
}
//-----------------------------------------------------------

inline std::string Color::ToStringRGB() const
{
	char buffer[32];
	sprintf(buffer, "%.3f,%.3f,%.3f", r, g, b);
	return buffer;
}

inline const Color& Color::FromStringRGB(const std::string& str)
{
	sscanf(str.c_str(), "%f,%f,%f", &r, &g, &b);
	return *this;
}

inline const Color& Color::FromStringRGBA(const std::string& str)
{
	sscanf(str.c_str(), "%f,%f,%f,%f", &r, &g, &b, &a);
	return *this;
}


inline void ColorModulate(Color& out, const Color& c1, const Color& c2)
{
	out.Set(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b, c1.a * c2.a);
}

inline const Color& 
ColorLerp(Color& out, const Color& c1, const Color& c2, float fLerp)
{
	out.Set(c1.r + fLerp * (c2.r - c1.r),
		c1.g + fLerp * (c2.g - c1.g),
		c1.b + fLerp * (c2.b - c1.b),
		c1.a + fLerp * (c2.a - c1.a));
	return out;
}

inline Color ParseColor(const std::string& str)
{
	float r, g, b;
	sscanf(str.c_str(), "%f,%f,%f", &r, &g, &b);
	return Color(r, g, b);
}



}// end of namespace


#endif