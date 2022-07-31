#pragma once
#include"Transform.h"
//ѭ�������½������
class CCDSolver
{
protected:
	std::vector<transform> mIKChain;
	unsigned int mNumSteps;
	float mThreshold;

public:
	CCDSolver();
	unsigned int Size();
	void Resize(unsigned int newSize);
	transform& operator[](unsigned int index);
	transform GetLocalTransform(unsigned int index);
	void SetLocalTransform(unsigned int index,
		const transform& t);
	transform GetGlobalTransform(unsigned int index);
	unsigned int GetNumSteps();
	void SetNumSteps(unsigned int numSteps);
	float GetThreshold();
	void SetThreshold(float value);
	bool Solve(const transform& target);

};