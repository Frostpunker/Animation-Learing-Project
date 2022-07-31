#define _CRT_SECURE_NO_WARNINGS
#include "TestMain.h"
#include "GLTFLoader.h"
#include "Uniform.h"
#include "glad.h"
#include"Blending.h"

void TestMain::Init() 
{
	//cpu skin and gpu skin
#if 0
	cgltf_data* gltf = LoadGLTFFile("Assets/Woman.gltf");
	mSkeleton = LoadSkeleton(gltf);
	mCPUMeshes = LoadMeshes(gltf);
	mGPUMeshes = LoadMeshes(gltf);
	std::vector<Clip> clips = LoadAnimationClips(gltf);
	mClips.resize(clips.size());
	FreeGLTFFile(gltf);
	
	

	BoneMap bones = RearrangeSkeleton(mSkeleton);
	for (unsigned int i = 0, size = (unsigned int)mCPUMeshes.size(); i < size; ++i)
	{
		RearrangeMesh(mCPUMeshes[i], bones);
	}

	for (unsigned int i = 0, size = (unsigned int)mGPUMeshes.size(); i < size; ++i)
	{
		RearrangeMesh(mGPUMeshes[i], bones);
	}

	for (unsigned int i = 0, size = (unsigned int)clips.size(); i < size; ++i)
	{
		mClips[i] = OptimizeClip(clips[i]);
	}

	for (unsigned int i = 0, size = (unsigned int)mClips.size(); i < size; ++i) 
	{
		RearrangeFastclip(mClips[i], bones);
	}

	for (unsigned int i = 0, size = (unsigned int)mGPUMeshes.size(); i < size; ++i) 
	{
		mGPUMeshes[i].UpdateOpenGLBuffers();
	}

	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mSkinnedShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");
	mDiffuseTexture = new Texture("Assets/Woman.png");

	mGPUAnimInfo.mAnimatedPose = mSkeleton.GetRestPose();
	mGPUAnimInfo.mPosePalette.resize(mSkeleton.GetRestPose().Size());
	mCPUAnimInfo.mAnimatedPose = mSkeleton.GetRestPose();
	mCPUAnimInfo.mPosePalette.resize(mSkeleton.GetRestPose().Size());

	mGPUAnimInfo.mModel.translate = vec3(-2, 0, 0);
	mCPUAnimInfo.mModel.translate = vec3(2, 0, 0);

	unsigned int numUIClips = (unsigned int)mClips.size();
	for (unsigned int i = 0; i < numUIClips; ++i) {
		if (mClips[i].GetName() == "Walking") {
			mCPUAnimInfo.mClip = i;
		}
		else if (mClips[i].GetName() == "Running") {
			mGPUAnimInfo.mClip = i;
		}
	}
#endif
    
	//IK
#if 1
	//����UI
	unsigned int ccd = (unsigned int)strlen("CCD") + 1;
	unsigned int fabrik = (unsigned int)strlen("FABRIK") + 1;
	IKName = new char* [2];
	IKName[0] = new char[ccd];
	IKName[1] = new char[fabrik];
	memset(IKName[0], 0, sizeof(char) * ccd);
	memset(IKName[1], 0, sizeof(char) * fabrik);
	sprintf(IKName[0], "CCD");
	sprintf(IKName[1], "FABRIK");
	//xzƽ����˶��켣Ϊһ��������
	//ģ�͸߶ȣ���yֵ�����˶���ʵʱ����
	mLastModelY = 0.0f;
	FastVectorTrack& motionTrack = mMotionTrack;
	VectorTrack mTrack;
	mTrack.Resize(5);
	mTrack.SetInterpolation(Interpolation::Linear);
	mTrack[0].mTime = 0.0f;
	mTrack[0].mValue[0] = 0; mTrack[0].mValue[2] = 1;
	mTrack[1].mTime = 1.0f;
	mTrack[1].mValue[0] = 0; mTrack[1].mValue[2] = 10;
	mTrack[2].mTime = 3.0f;
	mTrack[2].mValue[0] = 22; mTrack[2].mValue[2] = 10;
	mTrack[3].mTime = 4.0f;
	mTrack[3].mValue[0] = 22; mTrack[3].mValue[2] = 2;
	mTrack[4].mTime = 6.0f;
	mTrack[4].mValue[0] = 0; mTrack[4].mValue[2] = 1;
	motionTrack = OptimizeTrack(mTrack);

	//���ﶯ�����ݵļ���
	cgltf_data* gltf = LoadGLTFFile("Assets/Woman.gltf");
	mMeshes = LoadMeshes(gltf);
	mSkeleton = LoadSkeleton(gltf);
	std::vector<Clip> clips = LoadAnimationClips(gltf);
	FreeGLTFFile(gltf);

	//�����Ż�
	BoneMap bones = RearrangeSkeleton(mSkeleton);
	for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i)
	{
		RearrangeMesh(mMeshes[i], bones);
	}

	mClips.resize(clips.size());
	for (unsigned int i = 0, size = (unsigned int)clips.size(); i < size; ++i)
	{
		mClips[i] = OptimizeClip(clips[i]);
	}

	for (unsigned int i = 0, size = (unsigned int)mClips.size(); i < size; ++i)
	{
		RearrangeFastclip(mClips[i], bones);
	}

	//���澲̬����ļ���
	gltf = LoadGLTFFile("Assets/IKCourse.gltf");
	mIKCourse = LoadStaticMeshes(gltf);
	FreeGLTFFile(gltf);
	mCourseTexture = new Texture("Assets/uv.png");
	mTriangles = MeshesToTriangles(mIKCourse);

	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mSkinnedShader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");
	mDiffuseTexture = new Texture("Assets/Woman.png");

	mLeftLeg = new IKLeg(mSkeleton, "LeftUpLeg", "LeftLeg", "LeftFoot", "LeftToeBase",SolveMode::CCD);
	mLeftLeg->SetAnkleOffset(0.2f);
	mRightLeg = new IKLeg(mSkeleton, "RightUpLeg", "RightLeg", "RightFoot", "RightToeBase", SolveMode::CCD);
	mRightLeg->SetAnkleOffset(0.2f);

	//�ҽŵ�����Ĳ�������,�õ��ҽ���y���ϵ��˶�����
	//1Ϊ�����Ӵ���0Ϊ̧����ߵ�
	FastScalarTrack& rightTrack = mRightLeg->GetTrack();
	ScalarTrack rTrack;
	rTrack.SetInterpolation(Interpolation::Cubic);
	rTrack.Resize(4);
	rTrack[0].mValue[0] = 1;
	rTrack[1].mValue[0] = 0;
	rTrack[2].mValue[0] = 0;
	rTrack[3].mValue[0] = 1;
	rTrack[0].mTime = 0.0f;
	rTrack[1].mTime = 0.3f;
	rTrack[2].mTime = 0.7f;
	rTrack[3].mTime = 1.0f;
	rightTrack = OptimizeTrack(rTrack);

	//��ŵ�����Ĳ�������,�õ������y���ϵ��˶�����
	//1Ϊ�����Ӵ���0Ϊ̧����ߵ�
	FastScalarTrack& leftTrack = mLeftLeg->GetTrack();
	ScalarTrack lTrack;
	lTrack.SetInterpolation(Interpolation::Cubic);
	lTrack.Resize(4);
	lTrack[0].mValue[0] = 0;
	lTrack[1].mValue[0] = 1;
	lTrack[2].mValue[0] = 1;
	lTrack[3].mValue[0] = 0;
	lTrack[0].mTime = 0.0f;
	lTrack[1].mTime = 0.4f;
	lTrack[2].mTime = 0.6f;
	lTrack[3].mTime = 1.0f;
	leftTrack = OptimizeTrack(lTrack);

	mCurrentClip = 0;
	mCurrentPose = mSkeleton.GetRestPose();
	mSkeleton.GetDqInvBindPose(mDqInvBindPalette);
	
	mCurrentPoseVisual = new DebugDraw();
	mCurrentPoseVisual->FromPose(mCurrentPose);
	mCurrentPoseVisual->UpdateOpenGLBuffers();


	for (unsigned int i = 0, size = (unsigned int)mClips.size(); i < size; ++i) {
		std::string& clipName = mClips[i].GetName();
		if (clipName == "Walking") {
			mCurrentClip = i;
		}
		unsigned int nameLength = (unsigned int)clipName.length();
	}

	mShowIKPose = true;
	mShowCurrentPose = true;

	mShowEnvironment = true;
	mShowMesh = true;
	mDepthTest = false;
	mTimeMod = 1.0f;
	mSinkIntoGround = 0.15f;

	// Start the character clamped to the ground. Move down a little bit so it's not perfectly up
	//�Ӱ�������Ͷ��һ������
	Ray groundRay(vec3(mModel.translate.x, 11, mModel.translate.z));
	unsigned int numTriangles = (unsigned int)mTriangles.size();
	vec3 hitPoint;
	//��⵱ǰ����������ĸ��������ཻ
	for (unsigned int i = 0; i < numTriangles; ++i) {
		if (RaycastTriangle(groundRay, mTriangles[i], hitPoint)) {
			mModel.translate = hitPoint;
			break;
		}
	}
	//��ģ�͵�λ����Ϊ��������һ��,��Ϊ���������ĽӴ���
	mModel.translate.y -= mSinkIntoGround;
	mLastModelY = mModel.translate.y;

	mToeLength = 0.3f;
#endif

	//Dq test
#if 0
	cgltf_data* gltf = LoadGLTFFile("Assets/dq.gltf");
	mMeshes = LoadMeshes(gltf);
	mSkeleton = LoadSkeleton(gltf);
	mClips = LoadAnimationClips(gltf);
	FreeGLTFFile(gltf);

	mLBSShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");
	mDQShader = new Shader("Shaders/dqskinned.vert", "Shaders/lit.frag");
	mDiffuseTexture = new Texture("Assets/dq.png");

	mCurrentPose = mSkeleton.GetRestPose();

	mCurrentPose.GetDualQuaternionPalette(mDqPosePalette);
	mDqInvBindPalette = mSkeleton.GetDqInvBindPose();

	mCurrentPose.GetMatrixPalette(mLbPosePalette);
	mLbInvBindPalette = mSkeleton.GetInvBindPose();

	mCurrentClip = 0;
	mPlaybackTime = 0.0f;
#endif
}

void TestMain::Update(float deltaTime) {
//cpu skin and gpu skin
#if 0
	mCPUAnimInfo.mPlayback = mClips[mCPUAnimInfo.mClip].Sample(mCPUAnimInfo.mAnimatedPose, mCPUAnimInfo.mPlayback + deltaTime);
	mGPUAnimInfo.mPlayback = mClips[mGPUAnimInfo.mClip].Sample(mGPUAnimInfo.mAnimatedPose, mGPUAnimInfo.mPlayback + deltaTime);

	mGPUAnimInfo.mAnimatedPose.GetMatrixPalette(mGPUAnimInfo.mPosePalette);
	mCPUAnimInfo.mAnimatedPose.GetMatrixPalette(mCPUAnimInfo.mPosePalette);
	
	for (unsigned int i = 0, size = (unsigned int)mCPUAnimInfo.mPosePalette.size(); i < size; ++i)
	{
		//������Ƥ����
		//Kj = (Bj->M)^(-1) * Cj->M
		mCPUAnimInfo.mPosePalette[i] = mSkeleton.GetInvBindPose()[i] * mCPUAnimInfo.mPosePalette[i];
		mGPUAnimInfo.mPosePalette[i] = mSkeleton.GetInvBindPose()[i] * mGPUAnimInfo.mPosePalette[i];
	}

	for (unsigned int i = 0, size = (unsigned int)mCPUMeshes.size(); i < size; ++i) 
	{
		mCPUMeshes[i].CPUSkinTrans(mCPUAnimInfo.mPosePalette);
	}
	//GenMatrixPalette(mSkeleton, mGPUAnimInfo.mAnimatedPose, mGPUAnimInfo.mPosePalette);
#endif
	
	//IK
#if 1
	//������ʱ����IKģʽ
	mLeftLeg->SetSolveMode((SolveMode)IKMode);
	mRightLeg->SetSolveMode((SolveMode)IKMode);

	float rayHeight = 2.1f;
	unsigned int numTriangles = (unsigned int)mTriangles.size();
	deltaTime = deltaTime * mTimeMod;
	vec3 hitPoint;

	
	// �ݽ�ʱ�����Ҷ�Ƭ�ν��в���
	mWalkingTime += deltaTime * 0.3f;
	while (mWalkingTime > 6.0f) { mWalkingTime -= 6.0f; }

	//������һ֡��ģ�͸߶�
	float lastYPosition = mModel.translate.y;
	//����ģ�͵�x��z����
	//�������ģ�͵ĵ�ǰλ��
	vec3 currentPosition = mMotionTrack.Sample(mWalkingTime, true);
	//��������ģ����һ��λ��
	vec3 nextPosition = mMotionTrack.Sample(mWalkingTime + deltaTime * 0.3f, true);
	//y��ʱ���ֲ���
	currentPosition.y = lastYPosition;
	nextPosition.y = lastYPosition;
	//����ģ��x��z����
	mModel.translate = currentPosition;

	// ����ģ��������ռ�ĳ���
	vec3 direction = (nextPosition - currentPosition).GetNormalized();
	//��ģ����ת���µĳ������ת��
	quat newDirection = lookRotation(direction, vec3(0, 1, 0));
	//ת�����90������
	if (dot(mModel.rotation, newDirection) < 0.0f) {
		newDirection = newDirection * -1.0f;
	}
	mModel.rotation = Nlerp(mModel.rotation, newDirection, deltaTime * 10.0f);
	//�����ģ���µĳ���
	vec3 characterForward = rotate( mModel.rotation , vec3(0, 0, 1));


	//�������ģ�͵�yֵ
	Ray groundRay(vec3(mModel.translate.x, 11, mModel.translate.z));
	for (unsigned int i = 0; i < numTriangles; ++i) {
		if (RaycastTriangle(groundRay, mTriangles[i], hitPoint)) {
			// ��ģ����΢�����ƶ��Է�������չ�Ȳ�
			mModel.translate = hitPoint - vec3(0, mSinkIntoGround, 0);
			break;
		}
	}


	// �Ե�ǰƬ�ν��в��������Ҹ���hitpointӦ��IK���㣬����˫�ȵľֲ����ƣ��������һ֡������
	//�������һ֡������
	mPlaybackTime = mClips[mCurrentClip].Sample(mCurrentPose, mPlaybackTime + deltaTime);
	mCurrentPoseVisual->FromPose(mCurrentPose);
	//���㵱ǰ����ʱ�������Ƭ���еĽ���
	float normalizedTime = (mPlaybackTime - mClips[mCurrentClip].GetStartTime()) / mClips[mCurrentClip].GetDuration();
	if (normalizedTime < 0.0f) { std::cout << "should not be < 0\n"; normalizedTime = 0.0f; }
	if (normalizedTime > 1.0f) { std::cout << "should not be < 0\n"; normalizedTime = 1.0f; }
	//���ݲ��Ž��ȼ������ҽŵ�ǰ�����ľ���
	float leftMotion = mLeftLeg->GetTrack().Sample(normalizedTime, true);
	float rightMotion = mRightLeg->GetTrack().Sample(normalizedTime, true);


	//��������׵���������ʹӴ˴�Ͷ��Ĺ���
	vec3 worldLeftAnkle = combine(mModel, mCurrentPose.GetGlobalTransform(mLeftLeg->Ankle())).translate;
	Ray leftAnkleRay(worldLeftAnkle + vec3(0, 2, 0));
	vec3 predictiveLeftAnkle = worldLeftAnkle;

	//�����ҽ��׵���������ʹӴ˴�Ͷ��Ĺ���
	vec3 worldRightAnkle = combine(mModel, mCurrentPose.GetGlobalTransform(mRightLeg->Ankle())).translate;
	Ray rightAnkleRay(worldRightAnkle + vec3(0, 2, 0));
	vec3 predictiveRightAnkle = worldRightAnkle;


	// ʹ�����߼��������IK target
	vec3 groundReference = mModel.translate;
	for (unsigned int i = 0; i < numTriangles; ++i) {
		//������׵���������߼��
		if (RaycastTriangle(leftAnkleRay, mTriangles[i], hitPoint)) {
			//���������㹻���򽫽���λ�ø���Ϊ��ײ��
			if ((hitPoint - leftAnkleRay.origin).LenSq() < rayHeight * rayHeight) {
				worldLeftAnkle = hitPoint;
				//����ģ���ݽ���������
				if (hitPoint.y < groundReference.y) {
					groundReference = hitPoint - vec3(0, mSinkIntoGround, 0);
				}
			}
			//����׼��������λ��
			predictiveLeftAnkle = hitPoint;
		}
		//�ҽ���Ҳ�����
		if (RaycastTriangle(rightAnkleRay, mTriangles[i], hitPoint)) {
			if ((hitPoint - rightAnkleRay.origin).LenSq() < rayHeight * rayHeight) {
				worldRightAnkle = hitPoint;

				if (hitPoint.y < groundReference.y) {
					groundReference = hitPoint - vec3(0, mSinkIntoGround, 0);
				}
			}
			predictiveRightAnkle = hitPoint;
		}
	}


	// ��yֵ����һ�����Բ�ֵ�Է�����
	mModel.translate.y = mLastModelY;
	mModel.translate = Lerp(mModel.translate, groundReference, deltaTime * 10.0f);
	mLastModelY = mModel.translate.y;


	//�������Ų��˶�ѭ���и��ݸ߶�ֵ�����Բ�ֵ�Ų����˶�λ��
	worldLeftAnkle = Lerp(worldLeftAnkle, predictiveLeftAnkle, leftMotion);
	worldRightAnkle = Lerp(worldRightAnkle, predictiveRightAnkle, rightMotion);

	// ���ģ�ͺͽ��׵�λ�ú���ʼIK����
	mLeftLeg->SolveForLeg(mModel, mCurrentPose, worldLeftAnkle/*, worldLeftToe*/);
	mRightLeg->SolveForLeg(mModel, mCurrentPose, worldRightAnkle/*, worldRightToe*/);
	// ���½Ų�λ��
	Blend(mCurrentPose, mCurrentPose, mLeftLeg->GetAdjustedPose(), 1, mLeftLeg->Hip());
	Blend(mCurrentPose, mCurrentPose, mRightLeg->GetAdjustedPose(), 1, mRightLeg->Hip());


	//������ֺλ��
	//������׵�ȫ�ֱ任
	transform leftAnkleWorld = combine(mModel, mCurrentPose.GetGlobalTransform(mLeftLeg->Ankle()));
	transform rightAnkleWorld = combine(mModel, mCurrentPose.GetGlobalTransform(mRightLeg->Ankle()));
	//�����ֺ��ȫ��λ��
	vec3 worldLeftToe = combine(mModel, mCurrentPose.GetGlobalTransform(mLeftLeg->Toe())).translate;
	vec3 leftToeTarget = worldLeftToe;
	vec3 predictiveLeftToe = worldLeftToe;

	vec3 worldRightToe = combine(mModel, mCurrentPose.GetGlobalTransform(mRightLeg->Toe())).translate;
	vec3 rightToeTarget = worldRightToe;
	vec3 predictiveRightToe = worldRightToe;

	vec3 origin = leftAnkleWorld.translate;
	origin.y = worldLeftToe.y;
	//�ӽ�ֺͶ��һ������
	Ray leftToeRay(origin + characterForward * mToeLength + vec3(0, 1, 0));
	origin = rightAnkleWorld.translate;
	origin.y = worldRightToe.y;
	Ray rightToeRay = Ray(origin + characterForward * mToeLength + vec3(0, 1, 0));

	
	//��ֺ����ײ���
	float ankleRayHeight = 1.1f;
	for (unsigned int i = 0; i < numTriangles; ++i) {
		if (RaycastTriangle(leftToeRay, mTriangles[i], hitPoint)) {
			if ((hitPoint - leftToeRay.origin).LenSq() < ankleRayHeight * ankleRayHeight) {
				leftToeTarget = hitPoint;
			}
			predictiveLeftToe = hitPoint;
		}
		if (RaycastTriangle(rightToeRay, mTriangles[i], hitPoint)) {
			if ((hitPoint - rightToeRay.origin).LenSq() < ankleRayHeight * ankleRayHeight) {
				rightToeTarget = hitPoint;
			}
			predictiveRightToe = hitPoint;
		}
	}

	// ȷ����ֺλ��
	leftToeTarget = Lerp(leftToeTarget, predictiveLeftToe, leftMotion);
	rightToeTarget = Lerp(rightToeTarget, predictiveRightToe, rightMotion);

	// If the left or right toe hit, adjust the ankle rotation approrpaiteley
	//���ֺ����תУ��
	vec3 leftAnkleToCurrentToe = worldLeftToe - leftAnkleWorld.translate;
	vec3 leftAnkleToDesiredToe = leftToeTarget - leftAnkleWorld.translate;
	if (dot(leftAnkleToCurrentToe, leftAnkleToDesiredToe) > 0.00001f) {
		//������Ҫ����ת��
		quat ankleRotator = fromTo(leftAnkleToCurrentToe, leftAnkleToDesiredToe);
		//���׵ľֲ��任
		transform ankleLocal = mCurrentPose.GetLocalTransform(mLeftLeg->Ankle());
		//���׵�ȫ����ת
		quat worldRotatedAnkle = leftAnkleWorld.rotation * ankleRotator;
		//���׵ľֲ���ת
		quat localRotatedAnkle = worldRotatedAnkle * inverse(leftAnkleWorld.rotation);
		//���¾ֲ���ת
		ankleLocal.rotation = localRotatedAnkle * ankleLocal.rotation;
		mCurrentPose.SetLocalTransform(mLeftLeg->Ankle(), ankleLocal);
	}

	//�ҽ�ֺ����תУ��
	vec3 rightAnkleToCurrentToe = worldRightToe - rightAnkleWorld.translate;
	vec3 rightAnkleToDesiredToe = rightToeTarget - rightAnkleWorld.translate;
	if (dot(rightAnkleToCurrentToe, rightAnkleToDesiredToe) > 0.00001f) {
		quat ankleRotator = fromTo(rightAnkleToCurrentToe, rightAnkleToDesiredToe);
		transform ankleLocal = mCurrentPose.GetLocalTransform(mRightLeg->Ankle());

		quat worldRotatedAnkle = rightAnkleWorld.rotation * ankleRotator;
		quat localRotatedAnkle = worldRotatedAnkle * inverse(rightAnkleWorld.rotation);

		ankleLocal.rotation = localRotatedAnkle * ankleLocal.rotation;
		mCurrentPose.SetLocalTransform(mRightLeg->Ankle(), ankleLocal);
	}

	//��������ɫ��
	mCurrentPose.GetMatrixPalette(mPosePalette);
	for (int i = 0; i < mPosePalette.size(); i++)
	{
		mPosePalette[i] = mSkeleton.GetInvBindPose()[i] * mPosePalette[i];
	}
	//mCurrentPose.GetDualQuaternionPalette(mDqPosePalette);

#endif

	//Dq test
#if 0
	mPlaybackTime = mClips[mCurrentClip].Sample(mCurrentPose, mPlaybackTime + deltaTime);
	mCurrentPose.GetDualQuaternionPalette(mDqPosePalette);
	mCurrentPose.GetMatrixPalette(mLbPosePalette);
#endif
}

void TestMain::ImGui(nk_context* ctx) {
	nk_begin(ctx, "IK System", nk_rect(5.0f, 5.0f, 300.0f, 40.0f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR);

	static const float layout[] = { 90, 185 };
	nk_layout_row(ctx, NK_STATIC, 25, 2, layout);

	nk_label(ctx, "IK Mode:", NK_TEXT_LEFT);

	IKMode = nk_combo(ctx, (const char**)IKName, 2, IKMode, 25, nk_vec2(200, 200));

	nk_end(ctx);
}

void TestMain::Render(float inAspectRatio) 
{
	//cpu skin and gpu skin
#if 0
	mat4 projection = mat4::Perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = LookAt(vec3(0, 5, 7), vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 model;

	// CPU Skinned Mesh
	model = transformToMat4(mCPUAnimInfo.mModel);
	mStaticShader->Bind();
	Uniform<mat4>::Set(mStaticShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mStaticShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mStaticShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mStaticShader->GetUniform("light"), vec3(1, 1, 1));

	mDiffuseTexture->Set(mStaticShader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mCPUMeshes.size(); i < size; ++i) {
		mCPUMeshes[i].Bind(mStaticShader->GetAttribute("position"), mStaticShader->GetAttribute("normal"), mStaticShader->GetAttribute("texCoord"), -1, -1);
		mCPUMeshes[i].Draw();
		mCPUMeshes[i].UnBind(mStaticShader->GetAttribute("position"), mStaticShader->GetAttribute("normal"), mStaticShader->GetAttribute("texCoord"), -1, -1);
	}
	mDiffuseTexture->UnSet(0);
	mStaticShader->UnBind();

	// GPU Skinned Mesh
	model = transformToMat4(mGPUAnimInfo.mModel);
	mSkinnedShader->Bind();
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mSkinnedShader->GetUniform("light"), vec3(1, 1, 1));
	
	Uniform<mat4>::Set(mSkinnedShader->GetUniform("MatrixPalette"), mGPUAnimInfo.mPosePalette); 
	//Uniform<mat4>::Set(mSkinnedShader->GetUniform("pose"), mGPUAnimInfo.mPosePalette);
	//<mat4>::Set(mSkinnedShader->GetUniform("invBindPose"), mSkeleton.GetInvBindPose());

	mDiffuseTexture->Set(mSkinnedShader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mGPUMeshes.size(); i < size; ++i) {
		mGPUMeshes[i].Bind(mSkinnedShader->GetAttribute("position"), mSkinnedShader->GetAttribute("normal"), mSkinnedShader->GetAttribute("texCoord"), mSkinnedShader->GetAttribute("weights"), mSkinnedShader->GetAttribute("joints"));
		mGPUMeshes[i].Draw();
		mGPUMeshes[i].UnBind(mSkinnedShader->GetAttribute("position"), mSkinnedShader->GetAttribute("normal"), mSkinnedShader->GetAttribute("texCoord"), mSkinnedShader->GetAttribute("weights"), mSkinnedShader->GetAttribute("joints"));
	}
	mDiffuseTexture->UnSet(0);
	mSkinnedShader->UnBind();
#endif

	//IK
#if 1
	mat4 projection = mat4::Perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = LookAt(vec3(mModel.translate.x, 5, mModel.translate.z) + vec3(0, 5, 10), 
						vec3(mModel.translate.x, 0, mModel.translate.z) + vec3(0, 3, 0),
						vec3(0, 1, 0));
	mat4 model = transformToMat4(mModel);
	mat4 mvp = model * view * projection;
	mat4 vp = view * projection;
	if (mShowMesh) {
		Shader* characterShader = mSkinnedShader;

		characterShader->Bind();
		Uniform<mat4>::Set(characterShader->GetUniform("model"), model);
		Uniform<mat4>::Set(characterShader->GetUniform("view"), view);
		Uniform<mat4>::Set(characterShader->GetUniform("projection"), projection);
		Uniform<vec3>::Set(characterShader->GetUniform("light"), vec3(1, 1, 1));
		Uniform<mat4>::Set(characterShader->GetUniform("MatrixPalette"), mPosePalette);
		//Uniform<mat4>::Set(characterShader->GetUniform("invBindPose"), mSkeleton.GetInvBindPose());
		//��ż��Ԫ��ģʽ
		//Uniform<Dquat>::Set(characterShader->GetUniform("pose"), mDqPosePalette);
		//Uniform<Dquat>::Set(characterShader->GetUniform("invBindPose"), mDqInvBindPalette);
		mDiffuseTexture->Set(characterShader->GetUniform("tex0"), 0);
		for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i) {
			mMeshes[i].Bind(characterShader->GetAttribute("position"), characterShader->GetAttribute("normal"), characterShader->GetAttribute("texCoord"), characterShader->GetAttribute("weights"), characterShader->GetAttribute("joints"));
			mMeshes[i].Draw();
			mMeshes[i].UnBind(characterShader->GetAttribute("position"), characterShader->GetAttribute("normal"), characterShader->GetAttribute("texCoord"), characterShader->GetAttribute("weights"), characterShader->GetAttribute("joints"));
		}
		mDiffuseTexture->UnSet(0);
		characterShader->UnBind();
	}

	if (mShowEnvironment) {
		Shader* environmentShader = mStaticShader;
		environmentShader->Bind();
		Uniform<mat4>::Set(environmentShader->GetUniform("model"), mat4::Identity());
		Uniform<mat4>::Set(environmentShader->GetUniform("view"), view);
		Uniform<mat4>::Set(environmentShader->GetUniform("projection"), projection);
		Uniform<vec3>::Set(environmentShader->GetUniform("light"), vec3(1, 1, 1));
		mCourseTexture->Set(environmentShader->GetUniform("tex0"), 0);
		for (unsigned int i = 0, size = (unsigned int)mIKCourse.size(); i < size; ++i) {
			mIKCourse[i].Bind(environmentShader->GetAttribute("position"), environmentShader->GetAttribute("normal"), environmentShader->GetAttribute("texCoord"), -1, -1);
			mIKCourse[i].Draw();
			mIKCourse[i].UnBind(environmentShader->GetAttribute("position"), environmentShader->GetAttribute("normal"), environmentShader->GetAttribute("texCoord"), -1, -1);
		}
		mCourseTexture->UnSet(0);
		environmentShader->UnBind();
	}

	if (!mDepthTest) {
		glDisable(GL_DEPTH_TEST);
	}

	if (mShowCurrentPose) {
		mCurrentPoseVisual->UpdateOpenGLBuffers();
		mCurrentPoseVisual->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
	}

	if (mShowIKPose) {
		mLeftLeg->Draw(vp, vec3(1, 0, 0));
		mRightLeg->Draw(vp, vec3(0, 1, 0));
	}

	if (!mDepthTest) {
		glEnable(GL_DEPTH_TEST);
	}
#endif

#if 0
	//Dq test
	mat4 projection = mat4::Perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = LookAt(vec3(0, 3, 14), vec3(0, 0, 0), vec3(0, 1, 0));

	// Dual Quaternion
	transform model(vec3(2, 2, 0), quat(), vec3(1, 1, 1));
	mDQShader->Bind();
	Uniform<mat4>::Set(mDQShader->GetUniform("model"), transformToMat4(model));
	Uniform<mat4>::Set(mDQShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mDQShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mDQShader->GetUniform("light"), vec3(1, 1, 1));
	Uniform<Dquat>::Set(mDQShader->GetUniform("pose"), mDqPosePalette);
	Uniform<Dquat>::Set(mDQShader->GetUniform("invBindPose"), mDqInvBindPalette);

	mDiffuseTexture->Set(mDQShader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i) {
		mMeshes[i].Bind(mDQShader->GetAttribute("position"), mDQShader->GetAttribute("normal"), mDQShader->GetAttribute("texCoord"), mDQShader->GetAttribute("weights"), mDQShader->GetAttribute("joints"));
		mMeshes[i].Draw();
		mMeshes[i].UnBind(mDQShader->GetAttribute("position"), mDQShader->GetAttribute("normal"), mDQShader->GetAttribute("texCoord"), mDQShader->GetAttribute("weights"), mDQShader->GetAttribute("joints"));
	}
	mDiffuseTexture->UnSet(0);
	mDQShader->UnBind();

	// Linear Blend
	mLBSShader->Bind();
	model.translate.x = -2;
	model.translate.y = -2;
	Uniform<mat4>::Set(mDQShader->GetUniform("model"), transformToMat4(model));
	Uniform<mat4>::Set(mLBSShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mLBSShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mLBSShader->GetUniform("light"), vec3(1, 1, 1));
	Uniform<mat4>::Set(mLBSShader->GetUniform("pose"), mLbPosePalette);
	Uniform<mat4>::Set(mLBSShader->GetUniform("invBindPose"), mLbInvBindPalette);

	mDiffuseTexture->Set(mLBSShader->GetUniform("tex0"), 0);
	for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i) {
		mMeshes[i].Bind(mLBSShader->GetAttribute("position"), mLBSShader->GetAttribute("normal"), mLBSShader->GetAttribute("texCoord"), mLBSShader->GetAttribute("weights"), mLBSShader->GetAttribute("joints"));
		mMeshes[i].Draw();
		mMeshes[i].UnBind(mLBSShader->GetAttribute("position"), mLBSShader->GetAttribute("normal"), mLBSShader->GetAttribute("texCoord"), mLBSShader->GetAttribute("weights"), mLBSShader->GetAttribute("joints"));
	}
	mDiffuseTexture->UnSet(0);
	mLBSShader->UnBind();
#endif
}

void TestMain::Shutdown() {
//cpu skin and gpu skin
#if 0
	delete mStaticShader;
	delete mDiffuseTexture;
	delete mSkinnedShader;
	mClips.clear();
	mCPUMeshes.clear();
	mGPUMeshes.clear();
#endif

	//IK
#if 1
	delete mCurrentPoseVisual;
	delete mStaticShader;
	delete mDiffuseTexture;
	delete mSkinnedShader;
	delete mCourseTexture;
	delete mLeftLeg;
	delete mRightLeg;
	delete[] IKName[0];
	delete[] IKName[1];
	delete[] IKName;
	mClips.clear();
	mMeshes.clear();
#endif

#if 0
	//Dq test
	delete mDiffuseTexture;
	delete mLBSShader;
	delete mDQShader;
	mMeshes.clear();
	mClips.clear();
#endif
}