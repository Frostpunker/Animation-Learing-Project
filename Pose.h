#pragma once
#include"DualQuaternion.h"
#include<vector>
#include<iostream>

class Pose
{
protected:
	//存储每个关节的局部姿势
	//局部姿势指某关节在其父关节空间坐标下的坐标表示
	std::vector<transform> mJoints;
	//每个关节的父关节索引
	std::vector<int> mParents;

public:
	Pose() {}
	Pose(const Pose& p) { *this = p; }
	Pose& operator=(const Pose& p);
	Pose(size_t numJoints);
	//对所有成员调用Resize
	void Resize(size_t size);
	//返回关节的数量
	size_t Size();
	
	int GetParent(size_t index) { return mParents[index]; }
	void SetParent(size_t index, int parent)
	{
		mParents[index] = parent;
	}
	
	//局部姿势，指某关节到其父关节的变换
	transform GetLocalTransform(size_t index) { return mJoints[index]; }
	void SetLocalTransform(size_t index, const transform& trans)
	{
		mJoints[index] = trans;
	}
	
	//全局姿势，指某关节在模型空间下的变换
	transform GetGlobalTransform(size_t index);	
	//返回某个关节的全局姿势
	transform operator[](size_t index);
	

	void GetMatrixPalette(std::vector<mat4>& out);
	
	bool operator==(const Pose& other);
	bool operator!=(const Pose& other);

	//更新对偶四元数的接口
	void GetDualQuaternionPalette(std::vector<Dquat>& o);
	Dquat GetGlobalDualQuaternion(unsigned int i);
};
