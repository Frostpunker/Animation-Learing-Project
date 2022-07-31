#pragma once
#include "Vector3.h"
#include"Point4.h"
#include"Vector4.h"

template <typename T, size_t S>
class Matrix
{
public:

	Matrix()
	{
		memset(elements, 0, sizeof(T) * (S * S));
	}
	Matrix(T* data)
	{
		memcpy(elements, data, sizeof(T) * (S * S));
	}

	Matrix(T(*data)[S])
	{
		for (size_t row = 0; row < S; row++)
		{
			for (size_t col = 0; col < S; col++)
			{
				elements[row][col] = data[row][col];
			}
		}
		//T* temp;
		//temp = data[0];
		//elements = &temp;
	}

	Matrix(T r1c1, T r1c2, T r2c1, T r2c2)
	{
		if constexpr (S != 2)
		{
			static_assert(false, "Bad dimensionality");
		}
		elements[0][0] = r1c1;
		elements[0][1] = r1c2;
		elements[1][0] = r2c1;
		elements[1][1] = r2c2;
	}

	Matrix(T r1c1, T r1c2, T r1c3,
		T r2c1, T r2c2, T r2c3,
		T r3c1, T r3c2, T r3c3)
	{
		if constexpr (S != 3)
		{
			static_assert(false, "Bad dimensionality");
		}
		elements[0][0] = r1c1;
		elements[0][1] = r1c2;
		elements[0][2] = r1c3;
		elements[1][0] = r2c1;
		elements[1][1] = r2c2;
		elements[1][2] = r2c3;
		elements[2][0] = r3c1;
		elements[2][1] = r3c2;
		elements[2][2] = r3c3;
	}

	Matrix(T r1c1, T r1c2, T r1c3, T r1c4,
		T r2c1, T r2c2, T r2c3, T r2c4,
		T r3c1, T r3c2, T r3c3, T r3c4,
		T r4c1, T r4c2, T r4c3, T r4c4)
	{
		if constexpr (S != 4)
		{
			static_assert(false, "Bad dimensionality");
		}
		elements[0][0] = r1c1;
		elements[0][1] = r1c2;
		elements[0][2] = r1c3;
		elements[0][3] = r1c4;
		elements[1][0] = r2c1;
		elements[1][1] = r2c2;
		elements[1][2] = r2c3;
		elements[1][3] = r2c4;
		elements[2][0] = r3c1;
		elements[2][1] = r3c2;
		elements[2][2] = r3c3;
		elements[2][3] = r3c4;
		elements[3][0] = r4c1;
		elements[3][1] = r4c2;
		elements[3][2] = r4c3;
		elements[3][3] = r4c4;
	}

	//Matrix(T(&& data)[S][S]) : elements{ data } { data = nullptr; }
	Matrix& operator=(const Matrix& rhs)
	{
		memcpy(elements, rhs.elements, sizeof(elements));
		return *this;
	}
	Matrix& operator*=(T rhs)
	{
		for (auto& row : elements)
		{
			for (T& e : row)
			{
				e *= rhs;
			}
		}
		return *this;
	}
	Matrix operator*(T rhs) const
	{
		Matrix result = *this;
		return result *= rhs;
	}

	Matrix& operator*=(const Matrix& rhs)
	{
		return *this = *this * rhs;
	}

	Matrix operator+ (const Matrix& rhs)
	{
		Matrix result;
		for (int i = 0; i < S; i++)
		{
			for (int j = 0; j < S; j++)
			{
				result.elements[i][j] += rhs.elements[i][j];
			}
		}
		return result;
	}

	Matrix& operator+=(const Matrix& rhs)
	{
		return *this = *this + rhs;
	}

	Matrix operator- (const Matrix& rhs)
	{
		Matrix result;
		for (int i = 0; i < S; i++)
		{
			for (int j = 0; j < S; j++)
			{
				result.elements[i][j] -= rhs.elements[i][j];
			}
		}
		return result;
	}
	
	Matrix& operator-=(const Matrix& rhs)
	{
		return *this = *this + rhs;
	}


	Matrix operator*(const Matrix& rhs) const
	{
		Matrix result;
		for (size_t j = 0; j < S; j++)
		{
			for (size_t k = 0; k < S; k++)
			{
				T sum = (T)0.0;
				for (size_t i = 0; i < S; i++)
				{
					sum += elements[j][i] * rhs.elements[i][k];
				}
				result.elements[j][k] = sum;
			}
		}
		return result;
	}

	Matrix& TransPose()
	{
		for (size_t j = 0; j < S; j++)
		{
			for (size_t k = 0; k < S; k++)
			{
				elements[j][k] = elements[k][j];
			}
		}
		return *this;
	}

	Matrix GetTranspose() const
	{
		Matrix tp;
		for (size_t j = 0; j < S; j++)
		{
			for (size_t k = 0; k < S; k++)
			{
				tp.elements[j][k] = elements[k][j];
			}
		}
		return tp;
	}


	T Det()
	{
		if constexpr (S == 2)
		{
			return (T)(elements[0][0] * elements[1][1] - elements[0][1] * elements[1][0]);
		}
		if constexpr (S == 3)
		{
			return (T)(elements[0][0] * (elements[1][1] * elements[2][2] - elements[1][2] * elements[2][1])
				+ elements[0][1] * (elements[1][2] * elements[2][0] - elements[1][0] * elements[2][2])
				+ elements[0][2] * (elements[1][0] * elements[2][1] - elements[1][1] * elements[2][0])
				);
		}

		else if constexpr (S == 4)
		{
			return(T)(elements[0][0] *
				(elements[1][1] *
					(elements[2][2] * elements[3][3] - elements[2][3] * elements[3][2])
					+ elements[1][2] *
					(elements[2][3] * elements[3][1] - elements[2][1] * elements[3][3])
					+ elements[1][3] *
					(elements[2][1] * elements[3][2] - elements[2][2] * elements[3][1])
					)
				) -
				(elements[0][1] *
					(elements[1][0] *
						(elements[2][2] * elements[3][3] - elements[2][3] * elements[3][2])
						+ elements[1][2] *
						(elements[2][3] * elements[3][0] - elements[2][0] * elements[3][3])
						+ elements[1][3] *
						(elements[2][0] * elements[3][2] - elements[2][2] * elements[3][0])
						)
					) +
				(elements[0][2] *
					(elements[1][0] *
						(elements[2][1] * elements[3][3] - elements[2][3] * elements[3][1])
						+ elements[1][1] *
						(elements[2][3] * elements[3][0] - elements[2][0] * elements[3][3])
						+ elements[1][3] *
						(elements[2][0] * elements[3][1] - elements[2][1] * elements[3][0])
						)
					) -
				(elements[0][3] *
					(elements[1][0] *
						(elements[2][1] * elements[3][2] - elements[2][2] * elements[3][1])
						+ elements[1][1] *
						(elements[2][2] * elements[3][0] - elements[2][0] * elements[3][2])
						+ elements[1][2] *
						(elements[2][1] * elements[3][1] - elements[2][1] * elements[3][0])
						)
					);
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}

	constexpr static Matrix Identity()
	{
		if constexpr (S == 3)
		{
			return {
				(T)1.0,(T)0.0,(T)0.0,
				(T)0.0,(T)1.0,(T)0.0,
				(T)0.0,(T)0.0,(T)1.0,
			};
		}
		else if constexpr (S == 4)
		{
			return {
				(T)1.0,(T)0.0,(T)0.0,(T)0.0,
				(T)0.0,(T)1.0,(T)0.0,(T)0.0,
				(T)0.0,(T)0.0,(T)1.0,(T)0.0,
				(T)0.0,(T)0.0,(T)0.0,(T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
	constexpr static Matrix Scaling(T factor)
	{
		if constexpr (S == 3)
		{
			return{
				factor,(T)0.0,(T)0.0,
				(T)0.0,factor,(T)0.0,
				(T)0.0,(T)0.0,factor,
			};
		}
		else if constexpr (S == 4)
		{
			return {
				factor,(T)0.0,(T)0.0,(T)0.0,
				(T)0.0,factor,(T)0.0,(T)0.0,
				(T)0.0,(T)0.0,factor,(T)0.0,
				(T)0.0,(T)0.0,(T)0.0,(T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}

	}
	static Matrix RotationZ(T theta)
	{
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		if constexpr (S == 3)
		{
			return{
				 cosTheta, sinTheta, (T)0.0,
				-sinTheta, cosTheta, (T)0.0,
				(T)0.0,    (T)0.0,   (T)1.0,
			};
		}
		else if constexpr (S == 4)
		{
			return {
				 cosTheta, sinTheta, (T)0.0,(T)0.0,
				-sinTheta, cosTheta, (T)0.0,(T)0.0,
				(T)0.0,    (T)0.0,   (T)1.0,(T)0.0,
				(T)0.0,	   (T)0.0,   (T)0.0,(T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
	static Matrix RotationY(T theta)
	{
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		if constexpr (S == 3)
		{
			return{
				 cosTheta, (T)0.0,-sinTheta,
				 (T)0.0,   (T)1.0, (T)0.0,
				 sinTheta, (T)0.0, cosTheta
			};
		}
		else if constexpr (S == 4)
		{
			return {
				cosTheta, (T)0.0, -sinTheta,(T)0.0,
				(T)0.0,   (T)1.0, (T)0.0,   (T)0.0,
				sinTheta, (T)0.0, cosTheta, (T)0.0,
				(T)0.0,   (T)0.0, (T)0.0,   (T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
	static Matrix RotationX(T theta)
	{
		const T sinTheta = sin(theta);
		const T cosTheta = cos(theta);
		if constexpr (S == 3)
		{
			return{
				(T)1.0, (T)0.0,   (T)0.0,
				(T)0.0, cosTheta, sinTheta,
				(T)0.0,-sinTheta, cosTheta,
			};
		}
		else if constexpr (S == 4)
		{
			return {
				(T)1.0, (T)0.0,   (T)0.0,  (T)0.0,
				(T)0.0, cosTheta, sinTheta,(T)0.0,
				(T)0.0,-sinTheta, cosTheta,(T)0.0,
				(T)0.0, (T)0.0,   (T)0.0,  (T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}


	constexpr static Matrix Translation(T x, T y, T z)
	{
		if constexpr (S == 4)
		{
			return {
				(T)1.0,(T)0.0,(T)0.0,(T)0.0,
				(T)0.0,(T)1.0,(T)0.0,(T)0.0,
				(T)0.0,(T)0.0,(T)1.0,(T)0.0,
				x,     y,      z,    (T)1.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
	constexpr static Matrix Frustum(T l, T r, T b, T t, T n, T f)
	{
		if (l == r || t == b || n == f) {
			std::cout << "Invalid frustum\n";
			return Matrix::Identity(); // Error
		}

		if constexpr (S == 4)
		{
			return {
				(T)2.0 * n / (r - l),	(T)0.0,			(T)0.0,				(T)0.0,
				(T)0.0,			(T)2.0 * n / (t - b),	(T)0.0,				(T)0.0,
				(T)(r + l) / (r - l),			(T)(t + b) / (t - b),			(-(f + n)) / (f - n),		(T)-1.0,
				(T)0.0,			(T)0.0,			(-2 * n * f) / (f - n),	(T)0.0,
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}

	constexpr static Matrix Perspective(T fov, T asp, T n, T f)
	{
		if constexpr (S == 4)
		{
			T ymax = n * tanf(fov * PI / 360.0f);
			T xmax = ymax * asp;

			return Frustum(-xmax, xmax, -ymax, ymax, n, f);
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}

	constexpr static Matrix Ortho(T l, T r, T b, T t, T n, T f)
	{
		if constexpr (S == 4)
		{

			if (l == r || t == b || n == f) {
				return Matrix::Identity(); // Error
			}
			return {
				(T)2.0f / (r - l),		(T)0.0,		(T)0.0,		(T)0.0,
				(T)0.0,			(T)2.0f / (t - b),	(T)0.0,		(T)0.0,
				(T)0.0,			(T)0.0,				(T)-2.0f / (f - n),		(T)0.0,
				(T)-((r + l) / (r - l)), (T)-((t + b) / (t - b)), (T)-((f + n) / (f - n)), (T)1.0
			};
		}
		else
		{
			static_assert(false, "Bad dimensionality");
		}
	}
public:
	// [ row ][ col ]
	T elements[S][S];
};



template<typename T>
Vector3<T>& operator*=(Vector3<T>& lhs, const Matrix<T, 3>& rhs)
{
	return lhs = lhs * rhs;
}

template<typename T>
Vector3<T> operator*(const Vector3<T>& lhs, const Matrix<T, 3>& rhs)
{
	return{
		lhs.x * rhs.elements[0][0] + lhs.y * rhs.elements[1][0] + lhs.z * rhs.elements[2][0],
		lhs.x * rhs.elements[0][1] + lhs.y * rhs.elements[1][1] + lhs.z * rhs.elements[2][1],
		lhs.x * rhs.elements[0][2] + lhs.y * rhs.elements[1][2] + lhs.z * rhs.elements[2][2]
	};
}

template<typename T>
Vector4<T>& operator*=(Vector4<T>& lhs, const Matrix<T, 4>& rhs)
{
	return lhs = lhs * rhs;
}

template<typename T>
Vector4<T> operator*(const Vector4<T>& lhs, const Matrix<T, 4>& rhs)
{
	return{
		lhs.x * rhs.elements[0][0] + lhs.y * rhs.elements[1][0] + lhs.z * rhs.elements[2][0] + lhs.w * rhs.elements[3][0],
		lhs.x * rhs.elements[0][1] + lhs.y * rhs.elements[1][1] + lhs.z * rhs.elements[2][1] + lhs.w * rhs.elements[3][1],
		lhs.x * rhs.elements[0][2] + lhs.y * rhs.elements[1][2] + lhs.z * rhs.elements[2][2] + lhs.w * rhs.elements[3][2],
		lhs.x * rhs.elements[0][3] + lhs.y * rhs.elements[1][3] + lhs.z * rhs.elements[2][3] + lhs.w * rhs.elements[3][3]
	};
}

//用4x4矩阵变换一个3D向量
template<typename T>
Vector3<T> transformPoint(const Vector3<T>& lhs, const Matrix<T, 4>& rhs)
{
	Vector4<T> v4(lhs, 1.0f);
	Vector4<T> result = v4 * rhs;
	return Vector3<T>(result.x, result.y, result.z);
}

template<typename T>
Vector3<T> transformVector(const Vector3<T>& lhs, const Matrix<T, 4>& rhs)
{
	Vector4<T> v4(lhs, 0.0f);
	Vector4<T> result = v4 * rhs;
	return Vector3<T>(result.x, result.y, result.z);
}

/*
template<typename T>
Point4<T>& operator*=(Point4<T>& lhs, const Matrix<T, 4>& rhs)
{
	return lhs = lhs * rhs;
}*/

/*
template<typename T>
Point4<T> operator*(const Point4<T>& lhs, const Matrix<T, 4>& rhs)
{
	return{
		lhs.m_vec3.x * rhs.elements[0][0] + lhs.m_vec3.y * rhs.elements[1][0] + lhs.m_vec3.z * rhs.elements[2][0] + lhs.w * rhs.elements[3][0],
		lhs.m_vec3.x * rhs.elements[0][1] + lhs.m_vec3.y * rhs.elements[1][1] + lhs.m_vec3.z * rhs.elements[2][1] + lhs.w * rhs.elements[3][1],
		lhs.m_vec3.x * rhs.elements[0][2] + lhs.m_vec3.y * rhs.elements[1][2] + lhs.m_vec3.z * rhs.elements[2][2] + lhs.w * rhs.elements[3][2],
		lhs.m_vec3.x * rhs.elements[0][3] + lhs.m_vec3.y * rhs.elements[1][3] + lhs.m_vec3.z * rhs.elements[2][3] + lhs.w * rhs.elements[3][3]
	};
}*/

template<typename T>
Matrix<T, 4> LookAt(const Vector3<T>& pos, const Vector3<T>& target, const Vector3<T>& up)
{
	Vector3<T> f = (pos - target).GetNormalized();
	Vector3<T> r = cross(up, f); // Right handed
	if (r == Vector3<T>(0.0, 0.0, 0.0)) {
		return Matrix<T, 4>::Identity(); // Error
	}
	r.Normalize();
	Vector3<T> u = cross(f, r);
	u.Normalize(); // Right handed
	Vector3<T> t = Vector3<T>(
		-dot(r, pos),
		-dot(u, pos),
		-dot(f, pos)
		);

	return Matrix<T, 4>
		({ r.x, u.x, f.x, 0.0f,
		   r.y, u.y, f.y, 0.0f,
		   r.z, u.z, f.z, 0.0f,
		   t.x, t.y, t.z, 1.0f });
}





typedef Matrix<float, 3> mat3;
typedef Matrix<double, 3> mad3;
typedef Matrix<float, 4> mat4;
typedef Matrix<double, 4> mad4;