#include"RearrangeBones.h"
#include <list>
/*将所有关节排序，使得父关节在前，子关节在后
保存关节前后变换的映射关系，并据此重新排列Clip中的TransformTrack和
Mesh中的mInfluence向量*/
BoneMap RearrangeSkeleton(Skeleton& skeleton) 
{
	Pose& restPose = skeleton.GetRestPose();
	Pose& bindPose = skeleton.GetBindPose();
	unsigned int size = restPose.Size();
	if (size == 0) { return BoneMap(); }
	std::vector<std::vector<int>> hierarchy(size);
	std::list<int> process;
	for (unsigned int i = 0; i < size; ++i) 
	{
		int parent = restPose.GetParent(i);
		if (parent >= 0) 
		{
			hierarchy[parent].push_back((int)i);
		}
		else {
			process.push_back((int)i);
		}
	}
	//新位置到原位置的映射
	BoneMap mapForward;
	//原位置到新位置的映射
	BoneMap mapBackward;
	int index = 0;
	while (process.size() > 0) 
	{
		int current = *process.begin();
		process.pop_front();
		std::vector<int>& children = hierarchy[current];

		unsigned int numChildren = (unsigned int)children.size();
		for (unsigned int i = 0; i < numChildren; ++i) 
		{
			process.push_back(children[i]);
		}

		mapForward[index] = current;
		mapBackward[current] = index;
		index += 1;
	}

	mapForward[-1] = -1;
	mapBackward[-1] = -1;
	Pose newRestPose(size);
	Pose newBindPose(size);
	std::vector<std::string> newNames(size);
	for (unsigned int i = 0; i < size; ++i) {
		int thisBone = mapForward[i];
		newRestPose.SetLocalTransform(i, restPose.GetLocalTransform(thisBone));
		newBindPose.SetLocalTransform(i, bindPose.GetLocalTransform(thisBone));
		newNames[i] = skeleton.GetJointName(thisBone);

		int parent = mapBackward[bindPose.GetParent(thisBone)];
		newRestPose.SetParent(i, parent);
		newBindPose.SetParent(i, parent);
	}

	skeleton.Set(newRestPose, newBindPose, newNames);
	return mapBackward;
} // End of RearrangeSkeleton function

void RearrangeClip(Clip& clip, BoneMap& boneMap) 
{
	unsigned int size = clip.Size();

	for (unsigned int i = 0; i < size; ++i) 
	{
		int joint = (int)clip.GetIdAtIndex(i);
		unsigned int newJoint = (unsigned int)boneMap[joint];
		clip.SetIdAtIndex(i, newJoint);
	}
}

void RearrangeFastclip(FastClip& clip, BoneMap& boneMap) 
{
	unsigned int size = clip.Size();

	for (unsigned int i = 0; i < size; ++i) {
		int joint = (int)clip.GetIdAtIndex(i);
		unsigned int newJoint = (unsigned int)boneMap[joint];
		clip.SetIdAtIndex(i, newJoint);
	}
}

void RearrangeMesh(Mesh& mesh, BoneMap& boneMap) 
{
	std::vector<ivec4>& influences = mesh.GetInfluences();
	unsigned int size = (unsigned int)influences.size();
	for (unsigned int i = 0; i < size; ++i) 
	{
		influences[i].x = boneMap[influences[i].x];
		influences[i].y = boneMap[influences[i].y];
		influences[i].z = boneMap[influences[i].z];
		influences[i].w = boneMap[influences[i].w];
	}
	mesh.UpdateOpenGLBuffers();
}