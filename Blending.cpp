#include"Blending.h"

//检测在一个Pose中，search是否为parent的后裔关节
bool IsInHierarchy(Pose& pose, unsigned int parent,
	unsigned int search) 
{
	if (search == parent) 
	{
		return true;
	}
	int p = pose.GetParent(search);
	while (p >= 0) 
	{
		if (p == (int)parent) 
		{
			return true;
		}
		p = pose.GetParent(p);
	}
	return false;
}


void Blend(Pose& output, Pose& a, Pose& b, float t, int blendroot) 
{
	unsigned int numJoints = output.Size();
	for (unsigned int i = 0; i < numJoints; ++i) 
	{
		if (blendroot >= 0) 
		{
			//节点i不属于BlendRoot的后裔关节则略过
			//即output姿势中只有属于blendroot后裔的关节会进行混合
			if (!IsInHierarchy(output, (unsigned int)blendroot, i)) 
			{
				continue;
			}
		}

		output.SetLocalTransform(i, mix(a.GetLocalTransform(i), b.GetLocalTransform(i), t));
	}
}

Pose MakeAdditivePose(Skeleton& skeleton, FastClip& clip) 
{
	Pose result = skeleton.GetRestPose();
	//在t = 0处进行一次采样生成一个参考姿势
	clip.Sample(result, clip.GetStartTime());
	return result;
}

void Add(Pose& output, Pose& inPose, Pose& addPose, Pose& basePose, int blendroot) 
{
	unsigned int numJoints = addPose.Size();
	for (int i = 0; i < numJoints; ++i) {
		transform input = inPose.GetLocalTransform(i);
		transform additive = addPose.GetLocalTransform(i);
		transform additiveBase = basePose.GetLocalTransform(i);
		if (blendroot >= 0 &&
			!IsInHierarchy(addPose, blendroot, i)) {
			continue;
		}
		// outPose = inPose + (addPose - basePose)
		transform result(input.translate + (additive.translate - additiveBase.translate),
			(input.rotation * (inverse(additiveBase.rotation) * additive.rotation)).GetNormalized(),
			input.scale + (additive.scale - additiveBase.scale)
		);
		//以下算法同样可行
		//Transform delta = combine(inverse(additiveBase), additive);
		//Transform result(combine(delta, input));
		
		// outPose = inPose + (addPose - basePose)
		output.SetLocalTransform(i, result);
	}
}