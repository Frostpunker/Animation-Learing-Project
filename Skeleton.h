#pragma once
#include"Pose.h"
#include"DualQuaternion.h"
class Skeleton
{
protected:
	Pose mRestPose;
	Pose mBindPose;
	//�Է���������ʽ�洢ÿ���ؽ������ģ�Ϳռ����任����ÿ���ؽ�ȫ�����Ƶ���
	//��GLTF�ļ��ж�ȡȫ�����ƺ�����
	std::vector<mat4> mInvBindPose;
	//��ż��Ԫ����ʾ
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
	//���µĶ�ż��Ԫ���ӿ�
	//void UpdateDqInvBindPose();
	//std::vector<Dquat>& GetDqInvBindPose() { return mDqInvBindPose; }
	void GetDqInvBindPose(std::vector<Dquat>& outInvBndPose);

};

//void GenMatrixPalette(Skeleton& skeleton, Pose& curpose, std::vector<mat4> out);
