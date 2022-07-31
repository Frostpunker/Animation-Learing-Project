#pragma once
#include"Pose.h"
#include"DualQuaternion.h"
class Skeleton
{
protected:
	Pose mRestPose;
	Pose mBindPose;
	//以仿射矩阵的形式存储每个关节相对于模型空间的逆变换，即每个关节全局姿势的逆
	//从GLTF文件中读取全局姿势后求逆
	std::vector<mat4> mInvBindPose;
	//对偶四元数表示
	//std::vector<Dquat> mDqInvBindPose;
	std::vector<std::string> mJointNames;

	void UpdateInverseBindPose();

public:
	Skeleton() {}
	Skeleton(const Pose& rest, const Pose& bind,
		const std::vector<std::string>& names);
	void Set(const Pose& rest, const Pose& bind,
		const std::vector<std::string>& names);
	Pose& GetBindPose() { return mBindPose; }
	Pose& GetRestPose() { return mRestPose; }
	std::vector<mat4>& GetInvBindPose() { return mInvBindPose; }
	std::vector<std::string>& GetJointNames() { return mJointNames; }
	std::string& GetJointName(size_t index) { return mJointNames[index]; }
	//更新的对偶四元数接口
	//void UpdateDqInvBindPose();
	//std::vector<Dquat>& GetDqInvBindPose() { return mDqInvBindPose; }
	void GetDqInvBindPose(std::vector<Dquat>& outInvBndPose);

};

//void GenMatrixPalette(Skeleton& skeleton, Pose& curpose, std::vector<mat4> out);
