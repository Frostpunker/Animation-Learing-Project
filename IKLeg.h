#pragma once
#include "CCDSolver.h"
#include "FABRIKSolver.h"
#include "DebugDraw.h"
#include "Skeleton.h"
#include "Track.h"

enum class SolveMode {
	FABRIK,
	CCD 
};

class IKLeg {
protected:
	FastScalarTrack mPinTrack;
	FABRIKSolver mSolver;
	CCDSolver mCCDSolver;
	Pose mIKPose;
	SolveMode msolveMode;
	//由臀部、膝盖、脚踝、脚趾四个关节组成
	unsigned int mHipIndex;
	unsigned int mKneeIndex;
	unsigned int mAnkleIndex;
	unsigned int mToeIndex;

	DebugDraw* mLineVisuals;
	DebugDraw* mPointVisuals;

	// 脚踝关节到地面的距离
	float mAnkleToGroundOffset;
public:
	IKLeg(Skeleton& skeleton, const std::string& hip, const std::string& knee, const std::string& ankle, const std::string& toe,
		SolveMode solvemode);
	IKLeg();
	IKLeg(const IKLeg&);
	IKLeg& operator=(const IKLeg&);
	~IKLeg();
	void SetSolveMode(SolveMode solvemode);
	SolveMode GetSolveMode();
	void SolveForLeg(const transform& model, Pose& pose, const vec3& ankleTargetPosition);

	Pose& GetAdjustedPose();
	FastScalarTrack& GetTrack();

	void Draw(const mat4& vp, const vec3& legColor);

	unsigned int Hip();
	unsigned int Knee();
	unsigned int Ankle();
	unsigned int Toe();

	void SetAnkleOffset(float offset);
};

