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

//����ĳ���ؽ�����IKChain�е�ȫ�ֱ任
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

		//�ӵ����ڶ����ؽڿ�ʼ
		for (int j = (int)size - 2; j >= 0; --j)
		{
			effector = GetGlobalTransform(last).translate;
			//��j���ؽ���IKChain�е�ȫ�ֱ任����
			transform world = GetGlobalTransform(j);
			vec3 position = world.translate;
			quat rotation = world.rotation;
			//��ǰ�ؽڵ�endEffector������
			vec3 toEffector = effector - position;
			//��ǰ�ؽڵ�goal������
			vec3 toGoal = goal - position;
			//��toEffector��toGoal����ת��
			quat effectorToGoal;
			if (toGoal.LenSq() > 0.00001f)
			{
				effectorToGoal = fromTo(toEffector, toGoal);
			}

			//��goal����ϵ�µĵ���ת������ռ���
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