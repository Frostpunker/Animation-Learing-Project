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

//计算整个IKChain相对于首个关节的全局变换并计算整个IKChain的长度
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
			//需要平移的长度
			mLengths[i] = (world.translate - prev).Len();
		}
	}
	if (size > 0) 
	{
		mLengths[0] = 0.0f;
	}
}

//从IK链首端开始，逐次旋转每个关节，使之与求解出的worldChain匹配
void FABRIKSolver::WorldToIKChain()
{
	unsigned int size = Size();
	if (size == 0) { return; }
	//从第一个关节开始
	for (unsigned int i = 0; i < size - 1; ++i)
	{
		//当前关节在IK链中的全局变换
		transform world = GetGlobalTransform(i);
		//下一个关节在IK链中的全局变换
		transform next = GetGlobalTransform(i + 1);
		
		vec3 position = world.translate;
		quat rotation = world.translate;

		//在IKChain中的全局位置
		vec3 toNext = next.translate - position;
		//在世界坐标中的位置
		vec3 toDesired = mWorldChain[i + 1] - position;

		toNext = rotate(inverse(rotation), toNext);
		toDesired = rotate(inverse(rotation), toDesired);

		//从IKChain空间旋转到世界空间
		quat delta = fromTo(toNext, toDesired);
		mIKChain[i].rotation = delta * mIKChain[i].rotation;	
	}
}

//移动末梢节点到目标处，调整IK链
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

//移动首关节至原位，调整IK链
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
	//目标的位置
	vec3 goal = target.translate;
	//IK链首个关节的全局位置
	vec3 base = mWorldChain[0];
	//开始迭代求解
	for (unsigned int i = 0; i < mNumSteps; ++i) 
	{
		vec3 effector = mWorldChain[last];
		//如果末梢关节和目标足够近则求解成功
		if ((goal - effector).LenSq() < thresholdSq) 
		{
			WorldToIKChain();
			return true;
		}

		IterateBackward(goal);
		IterateForward(base);
	}
	//最后一次检测，如不通过则表示无解
	WorldToIKChain();
	vec3 effector = GetGlobalTransform(last).translate;
	if ((goal - effector).LenSq() < thresholdSq) {
		return true;
	}

	return false;
}