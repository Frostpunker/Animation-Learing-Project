#pragma once
/*双向迭代求解器*/
#include"Transform.h"
class FABRIKSolver
{
protected:
	std::vector<transform> mIKChain;
	unsigned int mNumSteps;
	float mThreshold;
	std::vector<vec3> mWorldChain;
	std::vector<float> mLengths;

protected:
	void IKChainToWorld();
	void IterateForward(const vec3& goal);
	void IterateBackward(const vec3& base);
	void WorldToIKChain();

public:
	FABRIKSolver();
	unsigned int Size();
	void Resize(unsigned int newSize);
	unsigned int GetNumSteps();
	void SetNumSteps(unsigned int numSteps);
	float GetThreshold();
	void SetThreshold(float value);
	transform GetLocalTransform(unsigned int index);
	void SetLocalTransform(unsigned int index,
		const transform& t);
	transform GetGlobalTransform(unsigned int index);
	bool Solve(const transform& target);
};