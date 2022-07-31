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
	//用于UI
	unsigned int ccd = (unsigned int)strlen("CCD") + 1;
	unsigned int fabrik = (unsigned int)strlen("FABRIK") + 1;
	IKName = new char* [2];
	IKName[0] = new char[ccd];
	IKName[1] = new char[fabrik];
	memset(IKName[0], 0, sizeof(char) * ccd);
	memset(IKName[1], 0, sizeof(char) * fabrik);
	sprintf(IKName[0], "CCD");
	sprintf(IKName[1], "FABRIK");
	//xz平面的运动轨迹为一个正方形
	//模型高度，即y值会在运动中实时计算
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

	//人物动画数据的加载
	cgltf_data* gltf = LoadGLTFFile("Assets/Woman.gltf");
	mMeshes = LoadMeshes(gltf);
	mSkeleton = LoadSkeleton(gltf);
	std::vector<Clip> clips = LoadAnimationClips(gltf);
	FreeGLTFFile(gltf);

	//数据优化
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

	//地面静态网格的加载
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

	//右脚到地面的采样数组,得到右脚在y轴上的运动曲线
	//1为与地面接触，0为抬到最高点
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

	//左脚到地面的采样数组,得到左脚在y轴上的运动曲线
	//1为与地面接触，0为抬到最高点
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
	//从半空向地面投射一条光线
	Ray groundRay(vec3(mModel.translate.x, 11, mModel.translate.z));
	unsigned int numTriangles = (unsigned int)mTriangles.size();
	vec3 hitPoint;
	//检测当前光线与地面哪个三角形相交
	for (unsigned int i = 0; i < numTriangles; ++i) {
		if (RaycastTriangle(groundRay, mTriangles[i], hitPoint)) {
			mModel.translate = hitPoint;
			break;
		}
	}
	//将模型的位置设为交点往下一点,作为脚掌与地面的接触点
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
		//计算蒙皮矩阵
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
	//在运行时设置IK模式
	mLeftLeg->SetSolveMode((SolveMode)IKMode);
	mRightLeg->SetSolveMode((SolveMode)IKMode);

	float rayHeight = 2.1f;
	unsigned int numTriangles = (unsigned int)mTriangles.size();
	deltaTime = deltaTime * mTimeMod;
	vec3 hitPoint;

	
	// 递进时步并且对片段进行采样
	mWalkingTime += deltaTime * 0.3f;
	while (mWalkingTime > 6.0f) { mWalkingTime -= 6.0f; }

	//保存上一帧的模型高度
	float lastYPosition = mModel.translate.y;
	//计算模型的x，z坐标
	//采样获得模型的当前位置
	vec3 currentPosition = mMotionTrack.Sample(mWalkingTime, true);
	//采样计算模型下一个位置
	vec3 nextPosition = mMotionTrack.Sample(mWalkingTime + deltaTime * 0.3f, true);
	//y暂时保持不变
	currentPosition.y = lastYPosition;
	nextPosition.y = lastYPosition;
	//更新模型x，z坐标
	mModel.translate = currentPosition;

	// 计算模型在世界空间的朝向
	vec3 direction = (nextPosition - currentPosition).GetNormalized();
	//将模型旋转至新的朝向的旋转量
	quat newDirection = lookRotation(direction, vec3(0, 1, 0));
	//转向大于90度则反向
	if (dot(mModel.rotation, newDirection) < 0.0f) {
		newDirection = newDirection * -1.0f;
	}
	mModel.rotation = Nlerp(mModel.rotation, newDirection, deltaTime * 10.0f);
	//计算出模型新的朝向
	vec3 characterForward = rotate( mModel.rotation , vec3(0, 0, 1));


	//最后计算出模型的y值
	Ray groundRay(vec3(mModel.translate.x, 11, mModel.translate.z));
	for (unsigned int i = 0; i < numTriangles; ++i) {
		if (RaycastTriangle(groundRay, mTriangles[i], hitPoint)) {
			// 将模型稍微向下移动以防过度伸展腿部
			mModel.translate = hitPoint - vec3(0, mSinkIntoGround, 0);
			break;
		}
	}


	// 对当前片段进行采样，并且根据hitpoint应用IK解算，调整双腿的局部姿势，计算出下一帧的姿势
	//计算出下一帧的姿势
	mPlaybackTime = mClips[mCurrentClip].Sample(mCurrentPose, mPlaybackTime + deltaTime);
	mCurrentPoseVisual->FromPose(mCurrentPose);
	//计算当前播放时间在这个片段中的进度
	float normalizedTime = (mPlaybackTime - mClips[mCurrentClip].GetStartTime()) / mClips[mCurrentClip].GetDuration();
	if (normalizedTime < 0.0f) { std::cout << "should not be < 0\n"; normalizedTime = 0.0f; }
	if (normalizedTime > 1.0f) { std::cout << "should not be < 0\n"; normalizedTime = 1.0f; }
	//根据播放进度计算左右脚当前与地面的距离
	float leftMotion = mLeftLeg->GetTrack().Sample(normalizedTime, true);
	float rightMotion = mRightLeg->GetTrack().Sample(normalizedTime, true);


	//计算左脚踝的世界坐标和从此处投射的光线
	vec3 worldLeftAnkle = combine(mModel, mCurrentPose.GetGlobalTransform(mLeftLeg->Ankle())).translate;
	Ray leftAnkleRay(worldLeftAnkle + vec3(0, 2, 0));
	vec3 predictiveLeftAnkle = worldLeftAnkle;

	//计算右脚踝的世界坐标和从此处投射的光线
	vec3 worldRightAnkle = combine(mModel, mCurrentPose.GetGlobalTransform(mRightLeg->Ankle())).translate;
	Ray rightAnkleRay(worldRightAnkle + vec3(0, 2, 0));
	vec3 predictiveRightAnkle = worldRightAnkle;


	// 使用射线检测来决定IK target
	vec3 groundReference = mModel.translate;
	for (unsigned int i = 0; i < numTriangles; ++i) {
		//从左脚踝到地面的射线检测
		if (RaycastTriangle(leftAnkleRay, mTriangles[i], hitPoint)) {
			//如果离地面足够近则将脚踝位置更新为碰撞点
			if ((hitPoint - leftAnkleRay.origin).LenSq() < rayHeight * rayHeight) {
				worldLeftAnkle = hitPoint;
				//避免模型陷进地面网格
				if (hitPoint.y < groundReference.y) {
					groundReference = hitPoint - vec3(0, mSinkIntoGround, 0);
				}
			}
			//左脚踝即将到达的位置
			predictiveLeftAnkle = hitPoint;
		}
		//右脚踝也是如此
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


	// 对y值进行一次线性插值以防跳变
	mModel.translate.y = mLastModelY;
	mModel.translate = Lerp(mModel.translate, groundReference, deltaTime * 10.0f);
	mLastModelY = mModel.translate.y;


	//在整个脚步运动循环中根据高度值来线性插值脚步的运动位置
	worldLeftAnkle = Lerp(worldLeftAnkle, predictiveLeftAnkle, leftMotion);
	worldRightAnkle = Lerp(worldRightAnkle, predictiveRightAnkle, rightMotion);

	// 求出模型和脚踝的位置后则开始IK解算
	mLeftLeg->SolveForLeg(mModel, mCurrentPose, worldLeftAnkle/*, worldLeftToe*/);
	mRightLeg->SolveForLeg(mModel, mCurrentPose, worldRightAnkle/*, worldRightToe*/);
	// 更新脚部位置
	Blend(mCurrentPose, mCurrentPose, mLeftLeg->GetAdjustedPose(), 1, mLeftLeg->Hip());
	Blend(mCurrentPose, mCurrentPose, mRightLeg->GetAdjustedPose(), 1, mRightLeg->Hip());


	//修正脚趾位置
	//计算脚踝的全局变换
	transform leftAnkleWorld = combine(mModel, mCurrentPose.GetGlobalTransform(mLeftLeg->Ankle()));
	transform rightAnkleWorld = combine(mModel, mCurrentPose.GetGlobalTransform(mRightLeg->Ankle()));
	//计算脚趾的全局位置
	vec3 worldLeftToe = combine(mModel, mCurrentPose.GetGlobalTransform(mLeftLeg->Toe())).translate;
	vec3 leftToeTarget = worldLeftToe;
	vec3 predictiveLeftToe = worldLeftToe;

	vec3 worldRightToe = combine(mModel, mCurrentPose.GetGlobalTransform(mRightLeg->Toe())).translate;
	vec3 rightToeTarget = worldRightToe;
	vec3 predictiveRightToe = worldRightToe;

	vec3 origin = leftAnkleWorld.translate;
	origin.y = worldLeftToe.y;
	//从脚趾投射一条射线
	Ray leftToeRay(origin + characterForward * mToeLength + vec3(0, 1, 0));
	origin = rightAnkleWorld.translate;
	origin.y = worldRightToe.y;
	Ray rightToeRay = Ray(origin + characterForward * mToeLength + vec3(0, 1, 0));

	
	//脚趾的碰撞检测
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

	// 确定脚趾位置
	leftToeTarget = Lerp(leftToeTarget, predictiveLeftToe, leftMotion);
	rightToeTarget = Lerp(rightToeTarget, predictiveRightToe, rightMotion);

	// If the left or right toe hit, adjust the ankle rotation approrpaiteley
	//左脚趾的旋转校正
	vec3 leftAnkleToCurrentToe = worldLeftToe - leftAnkleWorld.translate;
	vec3 leftAnkleToDesiredToe = leftToeTarget - leftAnkleWorld.translate;
	if (dot(leftAnkleToCurrentToe, leftAnkleToDesiredToe) > 0.00001f) {
		//脚踝需要的旋转量
		quat ankleRotator = fromTo(leftAnkleToCurrentToe, leftAnkleToDesiredToe);
		//脚踝的局部变换
		transform ankleLocal = mCurrentPose.GetLocalTransform(mLeftLeg->Ankle());
		//脚踝的全局旋转
		quat worldRotatedAnkle = leftAnkleWorld.rotation * ankleRotator;
		//脚踝的局部旋转
		quat localRotatedAnkle = worldRotatedAnkle * inverse(leftAnkleWorld.rotation);
		//更新局部旋转
		ankleLocal.rotation = localRotatedAnkle * ankleLocal.rotation;
		mCurrentPose.SetLocalTransform(mLeftLeg->Ankle(), ankleLocal);
	}

	//右脚趾的旋转校正
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

	//计算矩阵调色板
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
		//对偶四元数模式
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