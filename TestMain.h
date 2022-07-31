#pragma once
#include "Application.h"
#include <vector>
#include"DebugDraw.h"
#include "Clip.h"
#include "Skeleton.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include"IKLeg.h"
#include"Intersection.h"
#include "RearrangeBones.h"

#if 0
//cpu and gpu skin
struct AnimationInstance {
	Pose mAnimatedPose;
	std::vector <mat4> mPosePalette;
	unsigned int mClip;
	float mPlayback;
	transform mModel;

	inline AnimationInstance() : mClip(0), mPlayback(0.0f) { }
};
#endif

class TestMain : public Application {
protected:
	//cpu skin and gpu skin
#if 0
 
	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mSkinnedShader;
	std::vector<Mesh> mCPUMeshes;
	std::vector<Mesh> mGPUMeshes;
	Skeleton mSkeleton;
	std::vector<FastClip> mClips;

	AnimationInstance mGPUAnimInfo;
	AnimationInstance mCPUAnimInfo;

#endif

	//IK
#if 1
	Texture* mCourseTexture;
	std::vector<Mesh> mIKCourse;
	std::vector<Triangle> mTriangles;

	FastVectorTrack mMotionTrack;
	float mWalkingTime;

	transform mModel;
	std::vector<Mesh> mMeshes;
	Pose mCurrentPose;
	std::vector<mat4> mPosePalette;
	//为对偶四元数添加的属性
	std::vector<Dquat> mDqPosePalette;
	std::vector<Dquat> mDqInvBindPalette;
	float mSinkIntoGround;

	IKLeg* mLeftLeg;
	IKLeg* mRightLeg;
	float mToeLength;
	float mLastModelY;

	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mSkinnedShader;
	Skeleton mSkeleton;
	std::vector<FastClip> mClips;
	unsigned int mCurrentClip;
	float mPlaybackTime;
	DebugDraw* mCurrentPoseVisual;
	char** IKName;
	int IKMode;

	bool mShowIKPose;
	bool mShowCurrentPose;
	float mTimeMod;
	bool mDepthTest;
	bool mShowMesh;
	bool mShowEnvironment;
#endif

#if 0
	//Dq test
	Texture* mDiffuseTexture;
	Shader* mLBSShader;
	Shader* mDQShader;
	std::vector<Mesh> mMeshes;
	Skeleton mSkeleton;
	Pose mCurrentPose;

	std::vector<Dquat> mDqPosePalette;
	std::vector<Dquat> mDqInvBindPalette;
	std::vector<mat4> mLbPosePalette;
	std::vector<mat4> mLbInvBindPalette;

	std::vector<Clip> mClips;
	unsigned int mCurrentClip;
	float mPlaybackTime;
#endif

public:
	void Init() override;
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void ImGui(nk_context* context);
	void Shutdown();
};