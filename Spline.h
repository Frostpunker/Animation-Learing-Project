#pragma once
#include"Bezier.h"
#include"Hermite.h"

template<typename Spli>
class Spline
{
public:

	Spline(Spli _s) :s(_s) {}

	typename Spli::dataType Ierp(float t)
	{
		return s.Interpolate(t);
	}

private:
	Spli s;
};
