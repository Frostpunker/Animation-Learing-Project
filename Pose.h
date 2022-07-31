#pragma once
#include"DualQuaternion.h"
#include<vector>
#include<iostream>

class Pose
{
protected:
	//�洢ÿ���ؽڵľֲ�����
	//�ֲ�����ָĳ�ؽ����丸�ؽڿռ������µ������ʾ
	std::vector<transform> mJoints;
	//ÿ���ؽڵĸ��ؽ�����
	std::vector<int> mParents;

public:
	Pose() {}
	Pose(const Pose& p) { *this = p; }
	Pose& operator=(const Pose& p);
	Pose(size_t numJoints);
	//�����г�Ա����Resize
	void Resize(size_t size);
	//���عؽڵ�����
	size_t Size();
	
	int GetParent(size_t index) { return mParents[index]; }
	void SetParent(size_t index, int parent)
	{
		mParents[index] = parent;
	}
	
	//�ֲ����ƣ�ָĳ�ؽڵ��丸�ؽڵı任
	transform GetLocalTransform(size_t index) { return mJoints[index]; }
	void SetLocalTransform(size_t index, const transform& trans)
	{
		mJoints[index] = trans;
	}
	
	//ȫ�����ƣ�ָĳ�ؽ���ģ�Ϳռ��µı任
	transform GetGlobalTransform(size_t index);	
	//����ĳ���ؽڵ�ȫ������
	transform operator[](size_t index);
	

	void GetMatrixPalette(std::vector<mat4>& out);
	
	bool operator==(const Pose& other);
	bool operator!=(const Pose& other);

	//���¶�ż��Ԫ���Ľӿ�
	void GetDualQuaternionPalette(std::vector<Dquat>& o);
	Dquat GetGlobalDualQuaternion(unsigned int i);
};
