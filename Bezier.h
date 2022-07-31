#pragma once
#include"Vector2.h"
#include"Vector3.h"

template<typename T>
class Bezier 
{
public:
	typedef T dataType;
	T P1; // Point 1
	T C1; // Control 1
	T P2; // Point 2
	T C2; // Control 2

	Bezier(const T p1, const T c1, const T p2, const T c2) :
		P1(p1), C1(c1), P2(p2), C2(c2)
	{}

/*²åÖµ
P1((1 - t)^3) +
C1(3(1 - t)^2*t) +
C2(3(1 - t)*t^2) +
P2(t^3)
*/
	 T Interpolate(float t)
	{
		return P1 * ((1 - t) * (1 - t) * (1 - t)) +
			C1 * (3.0f * ((1 - t) * (1 - t)) * t) +
			C2 * (3.0f * (1 - t) * (t * t)) +
			P2 * (t * t * t);
	}

};


typedef Bezier<vec3> Bzi;