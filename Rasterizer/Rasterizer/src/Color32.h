#pragma once

#include <stdint.h>
#include "Math.h"

struct Color32
{

	union
	{
		uint32_t color;
		uint8_t colors[4];
		struct { uint8_t b, g, r, a; };
	};

	Color32()
	{
		color = 0xFFFFFFFF;
	}

	Color32(uint32_t color)
	{
		this->color = color;
	}

	Color32(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
	{
		colors[0] = b;
		colors[1] = g;
		colors[2] = r;
		colors[3] = a;
	}

	Color32(float a, float r, float g, float b)
	{
		colors[0] = (uint8_t)(Clamp(b, 0.0f, 1.0f) * 255.0f);
		colors[1] = (uint8_t)(Clamp(g, 0.0f, 1.0f) * 255.0f);
		colors[2] = (uint8_t)(Clamp(r, 0.0f, 1.0f) * 255.0f);
		colors[3] = (uint8_t)(Clamp(a, 0.0f, 1.0f) * 255.0f);
	}

	Color32& operator=(const Color32& c)
	{
		a = c.a;
		r = c.r;
		g = c.g;
		b = c.b;
		return *this;
	}

	bool operator==(const Color32& c)
	{
		return(color == c.color);
	}

	bool operator!=(const Color32& c)
	{
		return !operator==(c);
	}

	Color32 operator*(const Color32 c) const
	{
		Color32 ret;
		uint32_t ma = ((uint32_t)a);
		uint32_t mr = ((uint32_t)r);
		uint32_t mg = ((uint32_t)g);
		uint32_t mb = ((uint32_t)b);

		uint32_t ca = ((uint32_t)c.a);
		uint32_t cr = ((uint32_t)c.r);
		uint32_t cg = ((uint32_t)c.g);
		uint32_t cb = ((uint32_t)c.b);

		ret.a = (uint8_t)((ma * ca) / UINT8_MAX);
		ret.r = (uint8_t)((mr * cr) / UINT8_MAX);
		ret.g = (uint8_t)((mg * cg) / UINT8_MAX);
		ret.b = (uint8_t)((mb * cb) / UINT8_MAX);
		return ret;
	}

	Color32 operator*(const float flt) const
	{
		return Color32(
			((float)a / 255.0f) * flt,
			((float)r / 255.0f) * flt,
			((float)g / 255.0f) * flt,
			((float)b / 255.0f) * flt
		);
	}

	Color32& operator*=(const Color32 c)
	{
		uint32_t ma = ((uint32_t)a);
		uint32_t mr = ((uint32_t)r);
		uint32_t mg = ((uint32_t)g);
		uint32_t mb = ((uint32_t)b);

		uint32_t ca = ((uint32_t)c.a);
		uint32_t cr = ((uint32_t)c.r);
		uint32_t cg = ((uint32_t)c.g);
		uint32_t cb = ((uint32_t)c.b);

		a = (uint8_t)((ma * ca) / UINT8_MAX);
		r = (uint8_t)((mr * cr) / UINT8_MAX);
		g = (uint8_t)((mg * cg) / UINT8_MAX);
		b = (uint8_t)((mb * cb) / UINT8_MAX);
		return *this;
	}

	Color32 operator+(const Color32& c) const
	{
		return Color32((uint8_t)(Clamp(a + c.a, 0, 255)), 
			(uint8_t)(Clamp(r + c.r, 0, 255)), 
			(uint8_t)(Clamp(g + c.g, 0, 255)), 
			(uint8_t)(Clamp(b + c.b, 0, 255)));
	}

	Color32& operator+=(const Color32& c)
	{
		a = Clamp(a + c.a, 0, 255);
		r = Clamp(r + c.r, 0, 255);
		g = Clamp(g + c.g, 0, 255);
		b = Clamp(b + c.b, 0, 255);
		return *this;
	}

	bool operator==(const int32_t& c)
	{
		return(color == c);
	}

	bool operator!=(const int32_t& c)
	{
		return !operator==(c);
	}

	float GetFltR() const
	{
		return ((float)r / 255.0f);
	}

	float GetFltG() const
	{
		return ((float)g / 255.0f);
	}

	float GetFltB() const
	{
		return ((float)b / 255.0f);
	}

	float GetFltA() const
	{
		return ((float)a / 255.0f);
	}

	float GetAverage() const
	{
		float tab[4];
		tab[0] = GetFltR();
		tab[1] = GetFltG();
		tab[2] = GetFltB();
		tab[3] = GetFltA();
		return (tab[0] + tab[1] + tab[2] + tab[3]) * 0.25f;
	}

	float GetAverageNoAlpha() const
	{
		float tab[3];
		tab[0] = GetFltR();
		tab[1] = GetFltG();
		tab[2] = GetFltB();
		return (tab[0] + tab[1] + tab[2]) * 0.34f;
	}

	static Color32 AverageFour(const Color32* a)
	{
		return Color32(
			(uint8_t)(((uint32_t)a[0].a + (uint32_t)a[1].a + (uint32_t)a[2].a + (uint32_t)a[3].a) / 4),
			(uint8_t)(((uint32_t)a[0].r + (uint32_t)a[1].r + (uint32_t)a[2].r + (uint32_t)a[3].r) / 4),
			(uint8_t)(((uint32_t)a[0].g + (uint32_t)a[1].g + (uint32_t)a[2].g + (uint32_t)a[3].g) / 4),
			(uint8_t)(((uint32_t)a[0].b + (uint32_t)a[1].b + (uint32_t)a[2].b + (uint32_t)a[3].b) / 4)
			);
	}

	static float Distance(const Color32&a, const Color32&b)
	{
		return (float)fabs(b.GetFltA() - a.GetFltA()) + (float)fabs(b.GetFltR() - a.GetFltR()) + (float)fabs(b.GetFltG() - a.GetFltG()) + (float)fabs(b.GetFltB() - a.GetFltB());
	}

	static Color32 Lerp(const Color32& a, const Color32&b, float s)
	{
		Color32 col;
		col.a = (uint8_t)((float)a.a * (1.0f - s) + (float)b.a * s);
		col.r = (uint8_t)((float)a.r * (1.0f - s) + (float)b.r * s);
		col.g = (uint8_t)((float)a.g * (1.0f - s) + (float)b.g * s);
		col.b = (uint8_t)((float)a.b * (1.0f - s) + (float)b.b * s);
		return col;
	}

	static Color32 LerpNoAlpha(const Color32& a, const Color32&b, float s)
	{
		Color32 col;
		col.r = (uint8_t)((float)a.r * (1.0f - s) + (float)b.r * s);
		col.g = (uint8_t)((float)a.g * (1.0f - s) + (float)b.g * s);
		col.b = (uint8_t)((float)a.b * (1.0f - s) + (float)b.b * s);
		return col;
	}

	static Color32 MulNoAlpha(const Color32& a, const Color32&b)
	{
		uint8_t la = a.a;
		Color32 ret = a * b;
		ret.a = la;
		return ret;
	}
};