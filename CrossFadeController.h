#pragma once
#include"Skeleton.h"
#include"CrossFadeTarget.h"
class CrossFadeController {
protected:
	std::vector<CrossFadeTarget> mTargets;
	FastClip* mClip;
	float mTime;
	Pose mPose;
	Skeleton mSkeleton;
	bool mWasSkeletonSet;
public:
	CrossFadeController();
	CrossFadeController(Skeleton& skeleton);
	void SetSkeleton(Skeleton& skeleton);
	void Play(FastClip* target);
	void FadeTo(FastClip* target, float fadeTime);
	void Update(float dt);
	Pose& GetCurrentPose();
	FastClip* GetcurrentClip();
};