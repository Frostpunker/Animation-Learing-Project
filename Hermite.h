#pragma once
#include"Vector2.h"
#include"Vector3.h"

template<typename T>
class Hermite
{
public:
	typedef T dataType;
	T P1;//端点1
	T T1;//端点1的切向量
	T P2;//端点2
	T T2;//端点2的切向量

	Hermite(const T p1, const T t1, const T p2, const T t2) :
		P1(p1), T1(t1), P2(p2), T2(t2)
	{}

	T Interpolate(float t) 
	{
	float ttt = t * t * t;
	float tt = t * t;
	float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
	float h2 = -2.0f * ttt + 3.0f * tt;
	float h3 = ttt - 2.0f * tt + t;
	float h4 = ttt - tt;
	T result = P1 * h1 + P2 * h2 + T1 * h3 + T2 * h4;
	return result;
			/*
			P1 * ((1.0f + 2.0f * t) * ((1.0f - t) * (1.0f - t))) +
			T1 * (t * ((1.0f - t) * (1.0f - t))) +
			P2 * ((t * t) * (3.0f - 2.0f * t)) +
			T2 * ((t * t) * (t - 1.0f));*/
	}
};

//typedef Hermite<vec3> Her;