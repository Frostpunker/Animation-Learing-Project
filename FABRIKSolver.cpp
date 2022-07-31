#include"FABRIKSolver.h"

FABRIKSolver::FABRIKSolver() 
{
	mNumSteps = 15;
	mThreshold = 0.00001f;
}

unsigned int FABRIKSolver::GetNumSteps()
{
	return mNumSteps;
}

void FABRIKSolver::SetNumSteps(unsigned int numSteps)
{
	mNumSteps = numSteps;
}

float FABRIKSolver::GetThreshold() 
{
	return mThreshold;
}

void FABRIKSolver::SetThreshold(float value) 
{
	mThreshold = value;
}

unsigned int FABRIKSolver::Size() 
{
	return mIKChain.size();
}

void FABRIKSolver::Resize(unsigned int newSize) 
{
	mIKChain.resize(newSize);
	mWorldChain.resize(newSize);
	mLengths.resize(newSize);
}

transform FABRIKSolver::GetLocalTransform(unsigned int index) 
{
	return mIKChain[index];
}

void FABRIKSolver::SetLocalTransform(unsigned int index,const transform& t) 
{
	mIKChain[index] = t;
}

transform FABRIKSolver::GetGlobalTransform(unsigned int index) 
{
	unsigned int size = (unsigned int)mIKChain.size();
	transform world = mIKChain[index];
	for (int i = (int)index - 1; i >= 0; --i)
	{
		world = combine(mIKChain[i], world);
	}
	return world;
}

//��������IKChain������׸��ؽڵ�ȫ�ֱ任����������IKChain�ĳ���
void FABRIKSolver::IKChainToWorld() 
{
	unsigned int size = Size();
	for (unsigned int i = 0; i < size; ++i) 
	{
		transform world = GetGlobalTransform(i);
		mWorldChain[i] = world.translate;
		if (i >= 1) 
		{
			vec3 prev = mWorldChain[i - 1];
			//��Ҫƽ�Ƶĳ���
			mLengths[i] = (world.translate - prev).Len();
		}
	}
	if (size > 0) 
	{
		mLengths[0] = 0.0f;
	}
}

//��IK���׶˿�ʼ�������תÿ���ؽڣ�ʹ֮��������worldChainƥ��
void FABRIKSolver::WorldToIKChain()
{
	unsigned int size = Size();
	if (size == 0) { return; }
	//�ӵ�һ���ؽڿ�ʼ
	for (unsigned int i = 0; i < size - 1; ++i)
	{
		//��ǰ�ؽ���IK���е�ȫ�ֱ任
		transform world = GetGlobalTransform(i);
		//��һ���ؽ���IK���е�ȫ�ֱ任
		transform next = GetGlobalTransform(i + 1);
		
		vec3 position = world.translate;
		quat rotation = world.translate;

		//��IKChain�е�ȫ��λ��
		vec3 toNext = next.translate - position;
		//�����������е�λ��
		vec3 toDesired = mWorldChain[i + 1] - position;

		toNext = rotate(inverse(rotation), toNext);
		toDesired = rotate(inverse(rotation), toDesired);

		//��IKChain�ռ���ת������ռ�
		quat delta = fromTo(toNext, toDesired);
		mIKChain[i].rotation = delta * mIKChain[i].rotation;	
	}
}

//�ƶ�ĩ�ҽڵ㵽Ŀ�괦������IK��
void FABRIKSolver::IterateBackward(const vec3& goal) 
{
	int size = (int)Size();
	if (size > 0) 
	{
		mWorldChain[size - 1] = goal;
	}

	for (int i = size - 2; i >= 0; --i) {
		vec3 direction = (mWorldChain[i] - mWorldChain[i + 1]).GetNormalized();
		vec3 offset = direction * mLengths[i + 1];
		mWorldChain[i] = mWorldChain[i + 1] + offset;
	}
}

//�ƶ��׹ؽ���ԭλ������IK��
void FABRIKSolver::IterateForward(const vec3& base) {
	unsigned int size = Size();
	if (size > 0) {
		mWorldChain[0] = base;
	}

	for (int i = 1; i < size; ++i) {
		vec3 direction = (mWorldChain[i] - mWorldChain[i - 1]).GetNormalized();
		vec3 offset = direction * mLengths[i];
		mWorldChain[i] = mWorldChain[i - 1] + offset;
	}
}

bool FABRIKSolver::Solve(const transform& target)
{
	unsigned int size = Size();
	if (size == 0) { return false; }
	unsigned int last = size - 1;
	float thresholdSq = mThreshold * mThreshold;

	IKChainToWorld();
	//Ŀ���λ��
	vec3 goal = target.translate;
	//IK���׸��ؽڵ�ȫ��λ��
	vec3 base = mWorldChain[0];
	//��ʼ�������
	for (unsigned int i = 0; i < mNumSteps; ++i) 
	{
		vec3 effector = mWorldChain[last];
		//���ĩ�ҹؽں�Ŀ���㹻�������ɹ�
		if ((goal - effector).LenSq() < thresholdSq) 
		{
			WorldToIKChain();
			return true;
		}

		IterateBackward(goal);
		IterateForward(base);
	}
	//���һ�μ�⣬�粻ͨ�����ʾ�޽�
	WorldToIKChain();
	vec3 effector = GetGlobalTransform(last).translate;
	if ((goal - effector).LenSq() < thresholdSq) {
		return true;
	}

	return false;
}