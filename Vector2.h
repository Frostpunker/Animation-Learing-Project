#pragma once

template<typename T>
struct Vector2 {
	union {
		struct {
			T x;
			T y;
		};
		T v[2];
	};
	inline Vector2() : x(T(0)), y(T(0)) { }
	inline Vector2(T _x, T _y) :
		x(_x), y(_y) { }
	inline Vector2(T* fv) :
		x(fv[0]), y(fv[1]) { }
};

typedef Vector2<float> vec2;
typedef Vector2<int> ivec2;



