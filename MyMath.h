#pragma once
#include<math.h>
#include<iostream>
#include<vector>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932f;
constexpr float EPSILON = 0.0000001f;
constexpr float DEG2RAD = 0.0174533f;

template <typename T>
constexpr auto sq(const T& x)
{
	return x * x;
}

template<typename T>
T wrap_angle(T theta)
{
	const T modded = fmod(theta, (T)2.0 * (T)PI_D);
	return (modded > (T)PI_D) ?
		(modded - (T)2.0 * (T)PI_D) :
		modded;
}

template<typename T>
constexpr T Lerp(const T& src, const T& dst, float alpha)
{
	return src + (dst - src) * alpha;
}

template<typename T>
constexpr T to_rad(T deg)
{
	return deg * DEG2RAD;
}
