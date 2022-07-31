#pragma once
#include"Matrix.h"

template<typename T>
class Quat
{
public:

	inline Quat() :
		x(0.0f), y(0.0f), z(0.0f), w(1.0f) { }

	inline Quat(float _x, float _y, float _z, float _w)
		: x(_x), y(_y), z(_z), w(_w) {}

	inline Quat(const Vector3<T>& vec, T _w = 1)
		: vector(vec), scalar(_w) {}

	union
	{
		struct
		{
			T x;
			T y;
			T z;
			T w;
		};

		struct {
			Vector3<T> vector;
			T scalar;
		};

		T v[4];
	};

	Quat operator -() const
	{
		return Quat(-x, -y, -z, -w);
	}

	//Quat operator*(const Quat& rhs) const
	//{
		//return Quat(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
	//}
	
	Quat& operator=(const Quat& rhs)
	{
		if (&rhs == this) return *this;

		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
		return *this;
	}
	//Quat& operator=(const Quat& rhs) = default;

	inline T LenSq() const
	{
		return x * x + y * y + z * z + w * w;
	}

	T Len() const
	{
		if (LenSq() < EPSILON) return 0.0f;
		return sqrt(LenSq());
	}

	Quat& Normalize()
	{
		const T length = sqrtf(LenSq());
		if (length < EPSILON) return *this;
		float i_len = 1.0f / length;

		x *= i_len;
		y *= i_len;
		z *= i_len;
		w *= i_len;
		return *this;
	}

	Quat GetNormalized() const
	{
		const T length = sqrtf(LenSq());
		if (length < EPSILON) return *this;
		float i_len = 1.0f / length;

		return Quat(x * i_len, y * i_len, z * i_len, w * i_len);
	}
};

template<typename T>
Quat<T> operator+(const Quat<T>& lhs, const Quat<T>& rhs)
{
	return Quat<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

template<typename T>
Quat<T> operator-(const Quat<T>& lhs, const Quat<T>& rhs)
{
	return Quat<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

//标量乘法
template<typename T>
Quat<T> operator*(const Quat<T>& q, T n)
{
	return Quat<T>(q.x * n, q.y * n, q.z * n, q.w * n);
}

//四元数乘法
//连接Q1旋转到Q2旋转
template<typename T>
Quat<T> operator*(const Quat<T>& Q1, const Quat<T>& Q2)
{
	return Quat<T>(
		Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y + Q2.w * Q1.x,
		-Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x + Q2.w * Q1.y,
		Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w + Q2.w * Q1.z,
		-Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z + Q2.w * Q1.w
		);
}

//代数意义上的相等
template<typename T>
bool operator==(const Quat<T>& lhs, const Quat<T>& rhs)
{
	return (fabsf(lhs.x - rhs.x) <= EPSILON &&
		fabsf(lhs.y - rhs.y) <= EPSILON &&
		fabsf(lhs.z - rhs.z) <= EPSILON &&
		fabsf(lhs.w - rhs.w) <= EPSILON);
}

template<typename T>
bool operator!=(const Quat<T>& lhs, const Quat<T>& rhs)
{
	return !(lhs == rhs);
}

//四元数指数运算
template<typename T>
Quat<T> operator^(const Quat<T>& q, float f) {

	//防止除零
	if (fabs(q.scalar) < EPSILON) return Quat<T>(q);

	T angle = acosf(q.scalar); //theta * 0.5
	T newAngle = angle * f;

	T mult = sinf(newAngle) / sinf(angle);

	return Quat<T>(q.x * mult,
		q.y * mult,
		q.z * mult,
		cosf(newAngle)
		);
}

//两个四元数是否表示同一旋转
template<typename T>
bool sameOrientation(const Quat<T>& lhs, const Quat<T>& rhs)
{
	return (lhs == rhs) || (lhs == -rhs);
}

//返回共轭四元数
template<typename T>
Quat<T> conjugate(const Quat<T>& q)
{
	return Quat<T>(-q.x, -q.y, -q.z, q.w);
}

//四元数的逆
template<typename T>
Quat<T> inverse(const Quat<T>& q)
{
	const T lengthSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lengthSq < EPSILON) return Quat<T>();

	T s = 1.0 / lengthSq;
	return Quat<T>(-q.x * s,
		-q.y * s,
		-q.z * s,
		q.w * s
		);
}

//四元数点积
template<typename T>
T dot(const Quat<T>& lhs, const Quat<T>& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

//轴-角映射转换为四元数
template<typename T>
Quat<T> angleAxis(float angle, const Vector3<T>& axis)
{
	Vector3<T> norm = axis.GetNormalized();
	float s = sinf(angle * 0.5f);
	return Quat<T>(norm.x * s,
		norm.y * s,
		norm.z * s,
		cosf(angle * 0.5f)
		);
}

//根据两个向量创建一个四元数旋转算子
template<typename T>
Quat<T> fromTo(const Vector3<T>& from, const Vector3<T>& to)
{
	Vector3<T> f = from.GetNormalized();
	Vector3<T> t = to.GetNormalized();

	if (f == t) {
		return Quat<T>();
	}
	else if (f == t * -1.0f) {
		Vector3<T> ortho = Vector3<T>(1, 0, 0);
		if (fabsf(f.y) < fabsf(f.x)) {
			ortho = Vector3<T>(0, 1, 0);
		}
		if (fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabsf(f.x)) {
			ortho = Vector3<T>(0, 0, 1);
		}
		Vector3<T> axis = (cross(f, ortho)).GetNormalized();
		
		return Quat<T>(axis.x, axis.y, axis.z, 0);
	}
	//求出旋转轴
	Vector3<T> half = (f + t).GetNormalized();
	Vector3<T> axis = cross(f, half);
	return Quat<T>(axis.x, axis.y, axis.z, dot(f, half));
}

//通过四元数返回旋转轴
template<typename T>
Vector3<T> getAxis(const Quat<T>& quat)
{
	return Vector3<T>(quat.x, quat.y, quat.z).GetNormalized();
}

//通过四元数返回旋转角
template<typename T>
T getAngle(const Quat<T>& quat)
{
	return 2.0f * acosf(quat.w);
}

//用四元数对向量施加旋转
template<typename T>
Vector3<T> rotate(const Quat<T>& q, const Vector3<T>& v)
{
	return q.vector * 2.0f * dot(q.vector, v) +
		v * (q.scalar * q.scalar - dot(q.vector, q.vector)) +
		cross(q.vector, v) * 2.0f * q.scalar;
}

//代数意义上的四元数线性插值
template<typename T>
Quat<T> mix(const Quat<T>& from, const Quat<T>& to, float t) {
	return from * (1.0f - t) + to * t;
}

//四元数的球面插值
template<typename T>
Quat<T> Slerp(const Quat<T>& start, const Quat<T>& end, T t)
{
	//两者很相近时使用归一化线性插值
	if (fabsf(dot(start, end)) > 1.0f - EPSILON) {
		return Nlerp(start, end, t);
	}

	Quat<T> delta = inverse(start) * end; //四元数的“差”
	Quat<T> result = (delta ^ t) * start;
	return result.GetNormalized();
}

//四元数归一化线性插值
template<typename T>
Quat<T> Nlerp(const Quat<T>& from, const Quat<T>& to, T t) {
	Quat<T> result = from + (to - from) * t;
	return result.GetNormalized();
}

//
template<typename T>
Quat<T> lookRotation(const Vector3<T>& direction, const Vector3<T>& up) {
	// 获取一组正交基向量
	Vector3<T> f = direction.GetNormalized(); // Object Forward
	Vector3<T> u = up.GetNormalized(); // Desired Up
	Vector3<T> r = cross(u, f); // Object Right
	u = cross(f, r); // Object Up
	// From world forward to object forward
	Quat<T> worldToObject = fromTo(Vector3<T>(0, 0, 1), f);
	// what direction is the new object up?
	Vector3<T> objectUp = rotate(worldToObject,Vector3<T>(0, 1, 0));
	// From object up to desired up
	Quat<T> u2u = fromTo(objectUp, u);
	// Rotate to forward direction first
	// then twist to correct up
	Quat<T> result = worldToObject * u2u;


	return result.GetNormalized();
}

//四元数转换为4x4矩阵
template<typename T>
Matrix<T, 4> quatToMat4(const Quat<T>& q) {
	/*
	//对每个基向量施加四元数旋转
	Vector3<T> r = rotate(q, Vector3<T>(1, 0, 0));
	Vector3<T> u = rotate(q, Vector3<T>(0, 1, 0));
	Vector3<T> f = rotate(q, Vector3<T>(0, 0, 1));
	return Matrix<T, 4>(r.x, r.y, r.z, 0,
		u.x, u.y, u.z, 0,
		f.x, f.y, f.z, 0,
		0, 0, 0, 1
		);*/
	//直接给出代数解，要求必须为单位四元数
	return Matrix<T, 4>( 1 - 2 * (q.y * q.y + q.z * q.z), 2 * (q.x * q.y + q.z * q.w), 2 * (q.x * q.z - q.y * q.w), 0,
						 2 * (q.x * q.y - q.z * q.w), 1 - 2 * (q.x * q.x + q.z * q.z), 2 * (q.y * q.z + q.x * q.w), 0,
						 2 * (q.x * q.z + q.y * q.w), 2 * (q.y * q.z - q.x * q.w), 1 - 2 * (q.x * q.x + q.y * q.y), 0,
						 0,   0,   0,   1
						);
	
}

//4x4矩阵转换为四元数
template<typename T>
Quat<T> mat4ToQuat(const Matrix<T, 4>& m)
{
	Vector3<T> up = Vector3<T>(m.elements[1][0], m.elements[1][1], m.elements[1][2]).GetNormalized();
	Vector3<T> forward = Vector3<T>(m.elements[2][0], m.elements[2][1], m.elements[2][2]).GetNormalized();
	Vector3<T> right = cross(up, forward);
	up = cross(forward, right);
	return lookRotation(forward, up);
}

//3x3旋转矩阵转换为四元数的另一种优雅算法
//来自Nick Bobic
template<typename T>
Quat<T> mat3ToQuat_NB(const Matrix<T, 3>& m)
{
	T trace = m.elements[0][0] + m.elements[1][1] + m.elements[2][2];

	//检测主轴
	if (trace > EPSILON)
	{
		T s = sqrt(trace + 1.0f);
		T t = 0.5f / s;
		return Quat<T>((m.elements[2][1] - m.elements[1][2]) * t,
			(m.elements[0][2] - m.elements[2][0]) * t,
			(m.elements[1][0] - m.elements[0][1]) * t,
			0.5f * s);
	}

	//主轴为负
	else
	{
		int i = 0;
		T q[4];
		if (m.elements[1][1] > m.elements[0][0]) i = 1;
		if (m.elements[2][2] > m.elements[1][1]) i = 2;

		static const int next[3] = { 1,2,0 };
		int j = next[i];
		int k = next[j];

		T s = sqrtf((m.elements[i][i] - (m.elements[j][j] + m.elements[k][k])) + 1.0f);
		q[i] = s * 0.5f;

		T t;
		if (s != 0.0f) t = 0.5f / s;
		else t = s;

		q[3] = (m.elements[k][j] - m.elements[j][k]) * t;
		q[j] = (m.elements[j][i] + m.elements[i][j]) * t;
		q[k] = (m.elements[k][i] + m.elements[i][k]) * t;

		return Quat<T>(q[0], q[1], q[2], q[3]);

	}
}

typedef Quat<float> quat;

