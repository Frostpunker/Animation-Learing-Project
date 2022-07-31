#include"Skeleton.h"

Skeleton::Skeleton(const Pose& rest, const Pose& bind,
	const std::vector<std::string>& names) 
{
	Set(rest, bind, names);
}

//骨骼的绑定姿势更新时，其逆变换列表也会更新
void Skeleton::Set(const Pose& rest, const Pose& bind,
	const std::vector<std::string>& names) 
{
	mRestPose = rest;
	mBindPose = bind;
	mJointNames = names;
	UpdateInverseBindPose();
	//UpdateDqInvBindPose();
}
//计算绑定姿势全局变换的逆
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
//计算以对偶四元数表示的绑定姿势下的全局变换的逆
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
	//(Bj->M)^-1 矩阵的列表
	std::vector<mat4> invPosePalette = skeleton.GetInvBindPose();

	for (size_t i = 0; i < size; ++i)
	{
		transform cjm = curpose.GetGlobalTransform(i);
		transform invBjm = mat4ToTransform(invPosePalette[i]);
		out[i] = transformToMat4(combine(cjm, invBjm));
	}
}*/

