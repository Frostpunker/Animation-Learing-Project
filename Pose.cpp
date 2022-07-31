#include"Pose.h";

Pose::Pose(size_t numJoints) 
{
	Resize(numJoints);
}

Pose& Pose::operator=(const Pose& p) {
	//检测自赋值
	if (&p == this) 
	{
		return *this;
	}

	if (mParents.size() != p.mParents.size()) 
	{
		mParents.resize(p.mParents.size());
	}
	
	if (mJoints.size() != p.mJoints.size()) 
	{
		mJoints.resize(p.mJoints.size());
	}
	
	//快速复制
	if (mParents.size() != 0) 
	{
		memcpy(&mParents[0], &p.mParents[0], sizeof(int) * mParents.size());
	}
	
	if (mJoints.size() != 0) 
	{
		memcpy(&mJoints[0], &p.mJoints[0], sizeof(transform) * mJoints.size());
	}
	return *this;
}

void Pose::Resize(size_t size) 
{
	mParents.resize(size);
	mJoints.resize(size);
}

size_t Pose::Size() 
{
	return mJoints.size();
}

//计算全局姿势
//Pj->M = Pj->j-1 * Pj-1->j-2 * ... P0->M
transform Pose::GetGlobalTransform(size_t i) 
{
	transform result = mJoints[i];
	for (int p = mParents[i]; p >= 0; p = mParents[p]) 
	{
		result = combine(mJoints[p], result);
	}
	return result;
}

transform Pose::operator[](size_t index)
{
	return GetGlobalTransform(index);
}

//生成矩阵调色板
#if 1
void Pose::GetMatrixPalette(std::vector<mat4>& out) 
{
	int size = (int)Size();
	if ((int)out.size() != size) { out.resize(size); }

	//以下算法需要对关节进行排序，使得父关节在前，子关节在后
	int i = 0;
	for (; i < size; ++i) {
		int parent = mParents[i];
		if (parent > i) { break; }

		mat4 global = transformToMat4(mJoints[i]);
		if (parent >= 0) 
		{
			//直接乘已经计算出的父关节的全局变换即可
			global = global * out[parent];
		}
		out[i] = global;
	}
	
	for (; i < size; ++i) {
		transform t = GetGlobalTransform(i);
		out[i] = transformToMat4(t);
	}
}
#endif
#if 0
void Pose::GetMatrixPalette(std::vector<mat4>& out) 
{
	size_t size = Size();
	if (out.size() != size) 
	{
		out.resize(size);
	}
	for (size_t i = 0; i < size; ++i) 
	{
		transform t = GetGlobalTransform(i);
		out[i] = transformToMat4(t);
	}
}
#endif

bool Pose::operator==(const Pose& other) {
	
	if (&other == this) return true;
	//长度检测
	if (mJoints.size() != other.mJoints.size()) 
	{
		return false;
	}
	if (mParents.size() != other.mParents.size()) 
	{
		return false;
	}
	
	//逐元素对比
	size_t size = (size_t)mJoints.size();
	for (size_t i = 0; i < size; ++i) 
	{
		transform thisLocal = mJoints[i];
		transform otherLocal = other.mJoints[i];
		int thisParent = mParents[i];
		int otherParent = other.mParents[i];
		if (thisParent != otherParent) { return false; }
		if (thisLocal.translate != otherLocal.translate) 
		{
			return false;
		}
		if (thisLocal.rotation != otherLocal.rotation)
		{
		return false; 
		}
		if (thisLocal.scale != otherLocal.scale)
		{
		return false; 
		}
	}
	return true;
}

bool Pose::operator!=(const Pose& other) 
{
	return !(*this == other);
}

Dquat Pose::GetGlobalDualQuaternion(unsigned int index) 
{
	Dquat result = transformToDualQuat(mJoints[index]);
	for (int p = mParents[index]; p >= 0; p = mParents[p]) 
	{
		Dquat parent = transformToDualQuat(mJoints[p]);
		// Remember, multiplication is in reverse!
		result = result * parent;
	}
	return result;
}

void Pose::GetDualQuaternionPalette(std::vector<Dquat>& out) {
	unsigned int size = Size();
	if (out.size() != size) 
	{
		out.resize(size);
	}
	for (unsigned int i = 0; i < size; ++i) 
	{
		out[i] = GetGlobalDualQuaternion(i);
	}
}