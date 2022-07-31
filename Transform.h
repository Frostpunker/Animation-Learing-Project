#pragma once
#include"Quat.h"

//SQT�任
template<typename T>
class Transform
{
public:
	Transform() :
		translate(Vector3<T>(0, 0, 0)),
		rotation(Quat<T>(0, 0, 0, 1)),
		scale(Vector3<T>(1, 1, 1))
	{}

	Transform(const Vector3<T>& p, const Quat<T>& r, const Vector3<T>& s) :
		translate(p), rotation(r), scale(s) {}

	//Transform(const Transform& trans) = default;
public:
	Vector3<T> translate;
	Quat<T> rotation;
	Vector3<T> scale;
};

//���Ӵ�b��a�������任
template<typename T>
Transform<T> combine(const Transform<T>& a, const Transform<T>& b) {
	Transform<T> out;
	out.scale = a.scale * b.scale;
	out.rotation = b.rotation * a.rotation;
	//������ƽ��������������ƽ�������ĺ�
	//����a��ƽ���������䣬b��ƽ��������Ҫ�任
	out.translate = rotate(a.rotation,(a.scale * b.translate));//�任b��ƽ���������Ȱ�a��������������ת
	out.translate = a.translate + out.translate;//����������ӵõ�����ƽ������
	return out;
}

//��任
template<typename T>
Transform<T> inverse(const Transform<T>& t)
{
	Transform<T> inv;
	inv.rotation = inverse(t.rotation);//��ת����(������ת���ӣ�
	//ԭ�������ӵĵ���
	inv.scale.x = fabs(t.scale.x) < EPSILON ?
		0.0f : 1.0f / t.scale.x;
	inv.scale.y = fabs(t.scale.y) < EPSILON ?
		0.0f : 1.0f / t.scale.y;
	inv.scale.z = fabs(t.scale.z) < EPSILON ?
		0.0f : 1.0f / t.scale.z;
	//�ȷ��������ţ����ִ�з�����ת
	Vector3<T> invTrans = t.translate * -1.0f;
	inv.translate = rotate(inv.rotation, (inv.scale * invTrans));
	return inv;
}

//�Ա任���в�ֵ
template<typename T>
Transform<T> mix(const Transform<T>& a, const Transform<T>& b, T t) {
	Quat<T> bRot = b.rotation;
	if (dot(a.rotation, bRot) < 0.0f) {
		bRot = -bRot;
	}
	return Transform<T>(
		Lerp(a.translate, b.translate, t),
		Nlerp(a.rotation, bRot, t),
		Lerp(a.scale, b.scale, t));
}

//���任ת��Ϊ4x4�������
template<typename T>
Matrix<T, 4> transformToMat4(const Transform<T>& t)
{
	// ��ȡÿ������������ת��
	Vector3<T> x = rotate(t.rotation, Vector3<T>(1, 0, 0));
	Vector3<T> y = rotate(t.rotation, Vector3<T>(0, 1, 0));
	Vector3<T> z = rotate(t.rotation, Vector3<T>(0, 0, 1));
	// ���Ż�����
	x = x * t.scale.x;
	y = y * t.scale.y;
	z = z * t.scale.z;
	// ��ȡƽ����
	Vector3<T> p = t.translate;
	// Create matrix
	return Matrix<T, 4>(
		x.x, x.y, x.z, 0, // X basis (& Scale)
		y.x, y.y, y.z, 0, // Y basis (& scale)
		z.x, z.y, z.z, 0, // Z basis (& scale)
		p.x, p.y, p.z, 1 // Translation
		);
}

//�ֽ�4x4�ı任����
template<typename T>
Transform<T> mat4ToTransform(const Matrix<T, 4>& m)
{
	Transform<T> out;
	//ֱ����ȡ�������һ�е�ƽ����
	out.translate = Vector3<T>(m.elements[3][0], m.elements[3][1], m.elements[3][2]);
	//����תΪ��Ԫ��
	out.rotation = mat4ToQuat(m);
	//S * Q
	Matrix<T, 4> rotScaleMat(
		m.elements[0][0], m.elements[0][1], m.elements[0][2], 0,
		m.elements[1][0], m.elements[1][1], m.elements[1][2], 0,
		m.elements[2][0], m.elements[2][1], m.elements[2][2], 0,
		0, 0, 0, 1
	);
	//����ת����ת��Ϊ����
	//Q^(-1)
	Matrix<T, 4> invRotMat = quatToMat4(inverse(out.rotation));
	//S * Q * Q^(-1) = S
	Matrix<T, 4> scaleSkewMat = rotScaleMat * invRotMat;
	out.scale = Vector3<T>(scaleSkewMat.elements[0][0],
		scaleSkewMat.elements[1][1],
		scaleSkewMat.elements[2][2]
		);
	return out;
}

//�任һ����ά��
template<typename T>
Vector3<T> transformPoint(const Transform<T>& t, const Vector3<T>& v) {
	Vector3<T> out;
	out = rotate(t.rotation, (t.scale * v));
	out = t.translate + out;
	return out;
}

//�任һ����ά����
template<typename T>
Vector3<T> transformVector(const Transform<T>& t, const Vector3<T>& v) {
	Vector3<T> out;
	out = rotate(t.rotation, (t.scale * v));
	return out;
}

typedef Transform<float> transform;
