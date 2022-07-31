#include"Clip.h"
template TClip<TransformTrack>;
template TClip<FastTransformTrack>;

//���ݵ�ǰʱ��������ɵ�ǰ����
template <typename TRACK>
float TClip<TRACK>::Sample(Pose& outPose, float time)
{
	if (GetDuration() == 0.0f) 
	{
		return 0.0f;
	}
	time = AdjustTimeToFitRange(time);
	//�����Ĺؽ�����
	size_t size = mTracks.size();
	//�����ؽڽ��в���
	for (size_t i = 0; i < size; ++i) 
	{
		size_t j = mTracks[i].GetId(); // ��λĳ���ؽڵ�TransfromTrack
		transform local = outPose.GetLocalTransform(j);
		//���ĳ���ؽڽ��в�����ֵ
		//ͨ������ĳ���ؽ�������Ƭ�ε�TransformTrack��ȷ����ֵ������
		transform animated = mTracks[i].Sample(local, time, mLooping);
		//������ɸ������˵�TransfromTrack��ֵ�Ľ�������赱ǰ����
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

//����Clip�����п��õ�Track��У��Clip��ʱ��
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

//����һ���ض��ؽڵ�TransformTrack����ùؽڲ����ڣ��򴴽�һ��
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