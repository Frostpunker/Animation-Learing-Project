#pragma once
#include"Transform.h"
//用对偶四元数来表示旋转和平移
template<typename T>
struct DualQuaternion {
	union {
		struct {
			Quat<T> real;//存储旋转信息
			Quat<T> dual;//存储平移信息
		};
		T v[8];
	};
	inline DualQuaternion() : real(0, 0, 0, 1), dual(0, 0, 0,
		0) { }
	inline DualQuaternion(const Quat<T>& r, const Quat<T>& d) :
		real(r), dual(d) { }
	DualQuaternion& operator=(const DualQuaternion& src);

	DualQuaternion GetNormalized() const;
	DualQuaternion& Normalize();
};
/*
template<typename T>
DualQuaternion<T> operator+(const DualQuaternion<T>& l, const DualQuaternion<T>& r);
//标量乘法
template<typename T>
DualQuaternion<T> operator*(const DualQuaternion<T>& dq, T f);
//对偶四元数乘法
template<typename T>
DualQuaternion<T> operator*(const DualQuaternion<T>& l, const DualQuaternion<T>& r);
template<typename T>
bool operator==(const DualQuaternion<T>& l, const DualQuaternion<T>& r);
template<typename T>
bool operator!=(const DualQuaternion<T>& l, const DualQuaternion<T>& r);
template<typename T>
//只需要返回旋转部分的点积
T dot(const DualQuaternion<T>& l, const DualQuaternion<T>& r);
template<typename T>
DualQuaternion<T> conjugate(const DualQuaternion<T>& dq);
template<typename T>
DualQuaternion<T> transformToDualQuat(const Transform<T>& t);
template<typename T>
Transform<T> dualQuatToTransform(const DualQuaternion<T>& dq);
template<typename T>
Vector3<T> transformVector(const DualQuaternion<T>& dq, const Vector3<T>& v);
template<typename T>
Vector3<T> transformPoint(const DualQuaternion<T>& dq, const Vector3<T>& v);
*/
template<typename T>
DualQuaternion<T> operator+(const DualQuaternion<T>& l, const DualQuaternion<T>& r)
{
	return DualQuaternion<T>(l.real + r.real, l.dual + r.dual);
}

template<typename T>
DualQuaternion<T> operator*(const DualQuaternion<T>& dq, T f)
{
	return DualQuaternion<T>(dq.real * f, dq.dual * f);
}

template<typename T>
bool operator==(const DualQuaternion<T>& l, const DualQuaternion<T>& r)
{
	return l.real == r.real && l.dual == r.dual;
}

template<typename T>
bool operator!=(const DualQuaternion<T>& l, const DualQuaternion<T>& r)
{
	return l.real != r.real || l.dual != r.dual;
}
template<typename T>
inline DualQuaternion<T>& DualQuaternion<T>::operator=(const DualQuaternion& src)
{
	if (src == *this) return *this;
	real = src.real;
	dual = src.dual;
}

template<typename T>
DualQuaternion<T> operator*(const DualQuaternion<T>& l, const DualQuaternion<T>& r)
{
	DualQuaternion<T> lhs = l.GetNormalized();
	DualQuaternion<T> rhs = r.GetNormalized();
	return DualQuaternion<T>(lhs.real * rhs.real,
		lhs.real * rhs.dual +
		lhs.dual * rhs.real);
}

template<typename T>
T dot(const DualQuaternion<T>& l, const DualQuaternion<T>& r)
{
	return dot(l.real, r.real);
}

template<typename T>
DualQuaternion<T> conjugate(const DualQuaternion<T>& dq)
{
	return DualQuaternion<T>(conjugate(dq.real), conjugate(dq.dual));
}

template<typename T>
DualQuaternion<T> DualQuaternion<T>::GetNormalized() const
{
	T magSq = dot(this->real, this->real);
	if (magSq < EPSILON) {
		return DualQuaternion<T>();
	}
	//除以实部的模长
	T invMag = 1.0f / sqrtf(magSq);
	return DualQuaternion<T>(this->real * invMag, this->dual * invMag);
}

template<typename T>
DualQuaternion<T>& DualQuaternion<T>::Normalize()
{
	T magSq = dot(this->real, this->real);
	if (magSq < EPSILON) {
		return DualQuaternion<T>();
	}
	//除以实部的模长
	T invMag = 1.0f / sqrtf(magSq);
	this->real = this->real * invMag;
	this->dual = this->dual * invMag;
	return *this;
}

template<typename T>
DualQuaternion<T> transformToDualQuat(const Transform<T>& t)
{
	Quat<T> d(t.translate.x, t.translate.y, t.translate.z, 0);
	Quat<T> qr = t.rotation;
	Quat<T> qd = qr * d * 0.5f;
	return DualQuaternion<T>(qr, qd);
}

template<typename T>
DualQuaternion<T>  dualQuatToTransform(const DualQuaternion<T>& dq)
{
	Transform<T> result;
	result.rotation = dq.real;
	Quat<T> d = conjugate(dq.real) * (dq.dual * 2.0f);
	result.position = vec3(d.x, d.y, d.z);
	return result;
}

template<typename T>
Vector3<T> transformVector(const DualQuaternion<T>& dq, const Vector3<T>& v)
{
	return rotate(dq.real, v);
}

template<typename T>
Vector3<T> transformPoint(const DualQuaternion<T>& dq, const Vector3<T>& v)
{
	Quat<T> d = conjugate(dq.real) * (dq.dual * 2.0f);
	Vector3<T> t = Vector3<T>(d.x, d.y, d.z);
	return rotate(dq.real, v) + t;

}


typedef DualQuaternion<float> Dquat;