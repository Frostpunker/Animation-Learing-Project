#include"CCDSolver.h"

CCDSolver::CCDSolver() 
{
	mNumSteps = 15;
	mThreshold = 0.00001f;
}

unsigned int CCDSolver::Size()
{
	return mIKChain.size();
}

void CCDSolver::Resize(unsigned int newSize) 
{
	mIKChain.resize(newSize);
}

transform& CCDSolver::operator[](unsigned int index) 
{
		return mIKChain[index];
}

transform CCDSolver::GetLocalTransform(unsigned int index)
{
	return mIKChain[index];
}

void CCDSolver::SetLocalTransform(unsigned int index, const transform& t)
{
	mIKChain[index] = t;
}

unsigned int CCDSolver::GetNumSteps() 
{
	return mNumSteps;
}

void CCDSolver::SetNumSteps(unsigned int numSteps) 
{
	mNumSteps = numSteps;
}

float CCDSolver::GetThreshold() 
{
	return mThreshold;
}

void CCDSolver::SetThreshold(float value) 
{
	mThreshold = value;
}

//返回某个关节在其IKChain中的全局变换
transform CCDSolver::GetGlobalTransform(unsigned int x) 
{
	unsigned int size = (unsigned int)mIKChain.size();
	transform world = mIKChain[x];
	for (int i = (int)x - 1; i >= 0; --i) {
		world = combine(mIKChain[i], world);
	}
	return world;
}

bool CCDSolver::Solve(const transform& target)
{
	unsigned int size = Size();
	if (size == 0) { return false; }
	unsigned int last = size - 1;
	float thresholdSq = mThreshold * mThreshold;
	vec3 goal = target.translate;

	for (unsigned int i = 0; i < mNumSteps; ++i)
	{
		vec3 effector = GetGlobalTransform(last).translate;
		if ((goal - effector).LenSq() < thresholdSq)
		{
			return true;
		}

		//从倒数第二个关节开始
		for (int j = (int)size - 2; j >= 0; --j)
		{
			effector = GetGlobalTransform(last).translate;
			//第j个关节在IKChain中的全局变换数据
			transform world = GetGlobalTransform(j);
			vec3 position = world.translate;
			quat rotation = world.rotation;
			//当前关节到endEffector的向量
			vec3 toEffector = effector - position;
			//当前关节到goal的向量
			vec3 toGoal = goal - position;
			//从toEffector到toGoal的旋转量
			quat effectorToGoal;
			if (toGoal.LenSq() > 0.00001f)
			{
				effectorToGoal = fromTo(toEffector, toGoal);
			}

			//将goal坐标系下的点旋转回世界空间中
			quat worldRotated = rotation * effectorToGoal;
			//
			quat localRotate = worldRotated * inverse(rotation);
			mIKChain[j].rotation = localRotate * mIKChain[j].rotation;

			effector = GetGlobalTransform(last).translate;
			if ((goal - effector).LenSq() < thresholdSq) 
			{
				return true;
			}
		}
	}
	return false;
}