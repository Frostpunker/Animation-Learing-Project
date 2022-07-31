#include"Clip.h"
template TClip<TransformTrack>;
template TClip<FastTransformTrack>;

//根据当前时间采样生成当前姿势
template <typename TRACK>
float TClip<TRACK>::Sample(Pose& outPose, float time)
{
	if (GetDuration() == 0.0f) 
	{
		return 0.0f;
	}
	time = AdjustTimeToFitRange(time);
	//包含的关节数量
	size_t size = mTracks.size();
	//遍历关节进行采样
	for (size_t i = 0; i < size; ++i) 
	{
		size_t j = mTracks[i].GetId(); // 定位某个关节的TransfromTrack
		transform local = outPose.GetLocalTransform(j);
		//针对某个关节进行采样插值
		//通过访问某个关节在整个片段的TransformTrack来确定插值的两端
		transform animated = mTracks[i].Sample(local, time, mLooping);
		//最后生成根据两端的TransfromTrack插值的结果并赋予当前姿势
		outPose.SetLocalTransform(j, animated);
	}
	return time;
}

template <typename TRACK>
float TClip<TRACK>::AdjustTimeToFitRange(float inTime)
{
	if (mLooping) 
	{
		float duration = mEndTime - mStartTime;
		if (duration <= 0) { return 0.0f; }
		inTime = fmodf(inTime - mStartTime, duration);
		if (inTime < 0.0f) 
		{
			inTime += duration;
		}
		inTime = inTime + mStartTime;
	}
	else 
	{
		if (inTime < mStartTime) 
		{
			inTime = mStartTime;
		}
		if (inTime > mEndTime) 
		{
			inTime = mEndTime;
		}
	}
	return inTime;
}

//遍历Clip中所有可用的Track以校正Clip的时间
template <typename TRACK>
void TClip<TRACK>::RecalculateDuration()
{
	mStartTime = 0.0f;
	mEndTime = 0.0f;
	bool startSet = false;
	bool endSet = false;
	size_t tracksSize = mTracks.size();
	for (size_t i = 0; i < tracksSize; ++i) 
	{
		if (mTracks[i].IsValid()) 
		{
			float startTime = mTracks[i].GetStartTime();
			float endTime = mTracks[i].GetEndTime();
			if (startTime < mStartTime || !startSet) 
			{
				mStartTime = startTime;
				startSet = true;
			}
			if (endTime > mEndTime || !endSet) 
			{
				mEndTime = endTime;
				endSet = true;
			}
		}
	}
}

//检索一个特定关节的TransformTrack，如该关节不存在，则创建一个
template <typename TRACK>
TRACK& TClip<TRACK>::operator[](size_t joint) {
	for (int i = 0, s = mTracks.size(); i < s; ++i) 
	{
		if (mTracks[i].GetId() == joint) {
			return mTracks[i];
		}
	}
	mTracks.push_back(TRACK());
	mTracks[mTracks.size() - 1].SetId(joint);
	return mTracks[mTracks.size() - 1];
}

FastClip OptimizeClip(Clip& input) 
{
	FastClip result;

	result.SetName(input.GetName());
	result.SetLooping(input.GetLooping());
	unsigned int size = input.Size();
	for (unsigned int i = 0; i < size; ++i) 
	{
		unsigned int joint = input.GetIdAtIndex(i);
		result[joint] = OptimizeTransformTrack(input[joint]);
	}
	result.RecalculateDuration();

	return result;
}