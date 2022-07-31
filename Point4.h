#pragma once
//这个文件当前没有使用
/*
#include"MyMath.h"
#include"Vector3.h"
#include"Vector4.h"
#include<algorithm>

template<typename T>
class Point4
{
	friend class Vector4<T>;
public:
	Point4() : m_vec3(0.0f, 0.0f, 0.0f), w(0.0f) {};

	Point4(T _x, T _y, T _z, float _w = 0.0f) :
		m_vec3(_x, _y, _z), w(_w)
	{};

	Point4(const Vector3<T>& v3, float w = 0.0f)
		:
		m_vec3(v3),
		w(w)
	{};

	Point4(T _n, float _w = 0.0f) :Point4(_n), w(_w)
	{};

	template <typename T2>
	explicit operator Point4<T2>() const
	{
		return{ (T2)this->x,(T2)this->y,(T2)this->z,(T2)this->w };
	}


	template <typename T3>
	Point4& scale(const T3& n)
	{
		m_vec3.x *= n;
		m_vec3.y *= n;
		m_vec3.z *= n;

		return *this;
	}


	Point4<T> operator-() const
	{
		return Point4<T>(-this->x, -this->y, -this->z);
	}

	Point4& operator=(const Point4& rhs)
	{
		m_vec3.x = rhs.m_vec3.x;
		m_vec3.y = rhs.m_vec3.y;
		m_vec3.w = rhs.m_vec3.w;
		this->z = rhs.z;
		return *this;
	}

	Vector3<T> operator+(const Vector3<T>& rhs)
	{
		Vector3<T> temp(m_vec3);
		temp.x += rhs.x;
		temp.y += rhs.y;
		temp.z += rhs.z;

		return temp;
	}


	Vector3<T> operator-(const Vector3<T>& rhs)
	{
		Vector3<T> temp(m_vec3);
		temp.x -= rhs.x;
		temp.y -= rhs.y;
		temp.z -= rhs.z;

		return temp;
	}

	Vector4<T>	operator+(const Vector4<T>& rhs) const
	{
		Vector4<T> temp(m_vec3);
		temp.x += rhs.x;
		temp.y += rhs.y;
		temp.z += rhs.z;

		return temp;
	}

	Vector4<T>	operator-(const Vector4<T>& rhs) const
	{
		Vector4<T> temp(m_vec3);
		temp.x -= rhs.x;
		temp.y -= rhs.y;
		temp.z -= rhs.z;

		return temp;
	}



	//_Vec4	operator%( const _Vec4& rhs ) const
	//{
	//	return _Vec4(
	//		y * rhs.z - z * rhs.y,
	//		z * rhs.x - x * rhs.z,
	//		x * rhs.y - y * rhs.x );
	//}

public:
	Vector3<T> m_vec3;
	T w;
};*/