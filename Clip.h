#pragma once
#include"Pose.h"
#include"TransformTrack.h"

template<typename TRACK>
class TClip
{
protected:

	std::vector<TRACK> mTracks;//由若干个关节的TransformTracks组成的vector
	std::string mName;
	float mStartTime;
	float mEndTime;
	bool mLooping;

	float AdjustTimeToFitRange(float inTime);

public:
	TClip() : mName("No Name Given"), mStartTime(0.0f), 
		mEndTime(0.0f), mLooping(true) {}
	//TClip& operator=(TClip& clip) = default;
	size_t GetIdAtIndex(size_t index) { return mTracks[index].GetId(); }
	void SetIdAtIndex(size_t idx, size_t id) {	return mTracks[idx].SetId(id);	}
	size_t Size() { return (size_t)mTracks.size(); }
	std::string& GetName() { return mName; }
	void SetName(const std::string& inNewName) { mName = inNewName; }
	float GetDuration() { return mEndTime - mStartTime; }
	float GetStartTime() { return mStartTime; }
	float GetEndTime() { return mEndTime; }
	bool GetLooping() { return mLooping; }
	void SetLooping(bool inLooping) { mLooping = inLooping; }
	
	float Sample(Pose& outPose, float inTime);
	TRACK& operator[](size_t index);
	void RecalculateDuration();

};

typedef TClip<TransformTrack> Clip;
typedef TClip<FastTransformTrack> FastClip;


FastClip OptimizeClip(Clip& input);

