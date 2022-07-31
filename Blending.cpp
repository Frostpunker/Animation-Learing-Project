#include"Blending.h"

//�����һ��Pose�У�search�Ƿ�Ϊparent�ĺ���ؽ�
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
			//�ڵ�i������BlendRoot�ĺ���ؽ����Թ�
			//��output������ֻ������blendroot����Ĺؽڻ���л��
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
	//��t = 0������һ�β�������һ���ο�����
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
		//�����㷨ͬ������
		//Transform delta = combine(inverse(additiveBase), additive);
		//Transform result(combine(delta, input));
		
		// outPose = inPose + (addPose - basePose)
		output.SetLocalTransform(i, result);
	}
}