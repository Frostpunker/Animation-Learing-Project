#pragma once
#include"Clip.h"
#include"Pose.h"
#include"Skeleton.h"
struct CrossFadeTarget 
{
	Pose mPose;
	FastClip* mClip;
	float mTime;
	float mDuration;
	float mElapsed;
	inline CrossFadeTarget()
		: mClip(0), mTime(0.0f),
		mDuration(0.0f), mElapsed(0.0f) { }
	inline CrossFadeTarget(FastClip* target, Pose& pose, float dur)
		: mClip(target), mTime(target->GetStartTime()),
		mPose(pose), mDuration(dur), mElapsed(0.0f) { }
};
