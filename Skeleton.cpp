#include"Skeleton.h"

Skeleton::Skeleton(const Pose& rest, const Pose& bind,
	const std::vector<std::string>& names) 
{
	Set(rest, bind, names);
}

//�����İ����Ƹ���ʱ������任�б�Ҳ�����
void Skeleton::Set(const Pose& rest, const Pose& bind,
	const std::vector<std::string>& names) 
{
	mRestPose = rest;
	mBindPose = bind;
	mJointNames = names;
	UpdateInverseBindPose();
	//UpdateDqInvBindPose();
}
//���������ȫ�ֱ任����
void Skeleton::UpdateInverseBindPose() 
{
	unsigned int size = mBindPose.Size();
	mInvBindPose.resize(size);
	for (unsigned int i = 0; i < size; ++i) 
	{
		transform world = mBindPose.GetGlobalTransform(i);
		mInvBindPose[i] = transformToMat4(inverse(world));
	}
}
//�����Զ�ż��Ԫ����ʾ�İ������µ�ȫ�ֱ任����
/*
void Skeleton::UpdateDqInvBindPose()
{
	unsigned int size = mBindPose.Size();
	mDqInvBindPose.resize(size);
	for (unsigned int i = 0; i < size; ++i) 
	{
		Dquat world = mBindPose.GetGlobalDualQuaternion(i);
		mDqInvBindPose[i] = conjugate(world);
		//mDqInvBindPose[i] = transformToDualQuat(mat4ToTransform(mInvBindPose[i]));
	}
}*/

void Skeleton::GetDqInvBindPose(std::vector<Dquat>& outInvBndPose) {
	unsigned int size = mBindPose.Size();
	outInvBndPose.resize(size);

	for (unsigned int i = 0; i < size; ++i) {
		Dquat world = mBindPose.GetGlobalDualQuaternion(i);
		outInvBndPose[i] = conjugate(world);
	}
}
/*
void GenMatrixPalette(Skeleton& skeleton, Pose& curpose, std::vector<mat4> out)
{
	size_t size = curpose.Size();
	if (out.size() != size)
	{
		out.resize(size);
	}
	//(Bj->M)^-1 ������б�
	std::vector<mat4> invPosePalette = skeleton.GetInvBindPose();

	for (size_t i = 0; i < size; ++i)
	{
		transform cjm = curpose.GetGlobalTransform(i);
		transform invBjm = mat4ToTransform(invPosePalette[i]);
		out[i] = transformToMat4(combine(cjm, invBjm));
	}
}*/

