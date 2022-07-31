#pragma once
#include"MyMath.h"
#include<algorithm>

template<typename T>
class Vector3
{
public:
	inline Vector3() : x(0.0f), y(0.0f), z(0.0f) {};
	inline Vector3(T _x, T _y, T _z) :
		x(_x), y(_y), z(_z)
	{};

	inline Vector3(T _n) :x(_n), y(_n), z(_n)
	{};

	inline Vector3(T* v) :x(v[0]), y(v[1]), z(v[2])
	{};

	template <typename T2>
	explicit operator Vector3<T2>() const
	{
		return{ (T2)x,(T2)y,(T2)z };
	}

	inline T LenSq() const
	{
		return (x * x + y * y + z * z);
	}

	T Len() const
	{
		if (LenSq() < EPSILON) return 0.0f;
		return sqrt(LenSq());
	}

	Vector3& Normalize()
	{
		const T lengthSq = x * x + y * y + z * z;
		if (lengthSq < EPSILON) return *this;
		float i_len = 1.0f / sqrtf(lengthSq);
		x *= i_len;
		y *= i_len;
		z *= i_len;
		return *this;
	}

	Vector3	GetNormalized() const
	{
		Vector3 norm = *this;
		norm.Normalize();
		return norm;
	}

	Vector3 operator -() const
	{
		return Vector3(-x, -y, -z);
	}

	Vector3& operator=(const Vector3& rhs)
	{
		if (&rhs == this) return *this;

		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	template <typename T3>
	Vector3& scale(const T3& n)
	{
		x *= n;
		y *= n;
		z *= n;
		return *this;
	}

	Vector3	operator*(const T& rhs) const
	{
		return Vector3(*this) *= rhs;
	}

	Vector3	operator*(const Vector3& rhs) const
	{
		return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	Vector3& operator*=(const T& rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;

		return *this;
	}

	Vector3& operator+=(const Vector3& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Vector3& operator-=(const Vector3& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Vector3	operator-(const Vector3& rhs) const
	{
		return Vector3(*this) -= rhs;
	}
	//dot product
	T dot(const Vector3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	Vector3 operator+(const Vector3& rhs) const
	{
		return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vector3& operator/=(const T& rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}
	Vector3	operator/(const T& rhs) const
	{
		return Vector3(*this) /= rhs;
	}


	//bool	operator==(const Vector3& rhs) const
	//{
		//return x == rhs.x && y == rhs.y && z == rhs.z;
	//}

	//bool	operator!=(const Vector3& rhs) const
	//{
		//return !(*this == rhs);
	//}

	void LogVec()
	{
		std::cout << "(" << x << ", " << y << ", " << z << ") "
			<< std::endl;
	}
public:
	union
	{
		struct {
			T x;
			T y;
			T z;
		};
		T v[3];
	};

};

typedef Vector3<float> vec3;
typedef Vector3<double> dvec3;
typedef Vector3<int> ivec3;

template<typename T>
bool operator==(const Vector3<T>& lhs, const Vector3<T>& rhs) {
	Vector3<T> diff(lhs - rhs);
	return diff.LenSq() < EPSILON;
}

template<typename T>
bool operator!=(const Vector3<T>& lhs, const Vector3<T>& rhs) {
	return !(lhs == rhs);
}

template<typename T>
T GetCos(const Vector3<T>& lhs, const Vector3<T>& rhs)
{
	T sqMegL = lhs.LenSq();
	T sqMegR = rhs.LenSq();
	if (sqMegL < EPSILON || sqMegR < EPSILON) return 0.0f;

	T dotp = lhs.dot(rhs);
	T lenMul = sqrt(sqMegL) * sqrt(sqMegR);
	return dotp / lenMul;
}

template <typename T>
float angle(const Vector3<T>& lhs, const Vector3<T>& rhs)
{
	return acosf(GetCos(lhs, rhs));
}

template<typename T>
Vector3<T> project(const Vector3<T>& a, const Vector3<T>& b)
{
	T Megb = b.Len();
	if (Megb < EPSILON) return Vector3<T>();

	T scale = dot(a, b) / Megb;
	return b * scale;
}

template<typename T>
Vector3<T> reflect(const Vector3<T>& a, const Vector3<T>& b)
{
	return a - (project(a, b) * 2);
}

template<typename T>
T dot(const Vector3<T>& lhs, const Vector3<T>& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}


template<typename T>
Vector3<T> cross(const Vector3<T>& lhs, const Vector3<T>& rhs)
{
	return Vector3<T>(lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.x * rhs.y - lhs.y * rhs.x);
}


template<typename T>
Vector3<T> Slerp(const Vector3<T>& src, const Vector3<T>& dst, float alpha)
{
	if (alpha < 0.01f) {
		return Lerp(src, dst, alpha);
	}
	Vector3<T> from = src.GetNormalized();
	Vector3<T> to = dst.GetNormalized();
	T theta = angle(from, to);
	T sin_theta = sinf(theta);
	T a = sinf((1.0f - alpha) * theta) / sin_theta;
	T b = sinf(alpha * theta) / sin_theta;
	return from * a + to * b;
}

template<typename T>
Vector3<T> Nlerp(const Vector3<T>& src, const Vector3<T>& dst, float alpha)
{
	Vector3<T> linear = Lerp(src, dst, alpha);
	return linear.GetNormalized();
}

template<typename T>
Vector3<T> GetNormalized(const Vector3<T>& lhs, const Vector3<T>& rhs)
{
	Vector3<T> norm = *this;
	norm.Normalize();
	return norm;
}