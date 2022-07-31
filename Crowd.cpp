#include "Crowd.h"
#include "Uniform.h"

unsigned int Crowd::Size() {
	return mCurrentPlayTimes.size();
}

void Crowd::Resize(unsigned int size) 
{
	if (size > CROWD_MAX_ACTORS) 
	{
		size = CROWD_MAX_ACTORS;
	}

	mPositions.resize(size);
	mRotations.resize(size);
	mScales.resize(size, vec3(1, 1, 1));
	mFrames.resize(size);
	mTimes.resize(size);
	mCurrentPlayTimes.resize(size);
	mNextPlayTimes.resize(size);
}

transform Crowd::GetActor(unsigned int index) 
{
	return transform(
		mPositions[index],
		mRotations[index],
		mScales[index]);
}

void Crowd::SetActor(unsigned int index, const transform& t) 
{
	mPositions[index] = t.translate;
	mRotations[index] = t.rotation;
	mScales[index] = t.scale;
}

float Crowd::AdjustTime(float time, float start, float end, bool looping) 
{
	if (looping) 
	{
		time = fmodf(time - start, end - start);
		if (time < 0.0f) 
		{
			time += end - start;
		}
		time = time + start;
	}
	else 
	{
		if (time < start) 
		{
			time = start;
		}
		if (time > end) 
		{
			time = end;
		}
	}
	return time;
}

//更新人群的动画播放时间
void Crowd::UpdatePlaybackTimes(float deltaTime, bool looping, float start, float end) 
{
	unsigned int size = mCurrentPlayTimes.size();
	for (unsigned int i = 0; i < size; ++i) 
	{
		float time = mCurrentPlayTimes[i] + deltaTime;
		mCurrentPlayTimes[i] = AdjustTime(time, start, end, looping);
		//计算下一个时步
		time = mCurrentPlayTimes[i] + deltaTime;
		mNextPlayTimes[i] = AdjustTime(time, start, end, looping);
	}
}

//计算一个采样区间两端所属的帧
void Crowd::UpdateFrameIndices(float start, float duration, unsigned int texWidth) 
{
	unsigned int size = mCurrentPlayTimes.size();
	for (unsigned int i = 0; i < size; ++i) 
	{
		//当前相位
		float thisNormalizedTime = (mCurrentPlayTimes[i] - start) / duration;
		//计算当前所属帧(自动向下取整)
		unsigned int thisFrame = (unsigned int)(thisNormalizedTime * (float)(texWidth - 1));
		//计算下一时步的相位和所属帧
		float nextNormalizedTime = (mNextPlayTimes[i] - start) / duration;
		unsigned int nextFrame = (unsigned int)(nextNormalizedTime * (float)(texWidth - 1));

		mFrames[i].x = thisFrame;
		mFrames[i].y = nextFrame;
	}
}

//将两个采样点都规范到帧后求出当前播放时间在一个插值区间的相位
void Crowd::UpdateInterpolationTimes(float start, float duration, unsigned int texWidth) 
{
	unsigned int size = mCurrentPlayTimes.size();
	for (unsigned int i = 0; i < size; ++i) 
	{
		//检测两个时步是否在同一帧内
		if (mFrames[i].x == mFrames[i].y) 
		{
			mTimes[i] = 1.0f;
			continue;
		}
		//规范化到帧之后的相位
		float thisT = (float)mFrames[i].x / (float)(texWidth - 1);
		float thisTime = start + duration * thisT;
		float nextT = (float)mFrames[i].y / (float)(texWidth - 1);
		float nextTime = start + duration * nextT;

		if (nextTime < thisTime) 
		{
			nextTime += duration;
		}

		float frameDuration = nextTime - thisTime;
		mTimes[i] = (mCurrentPlayTimes[i] - thisTime) / frameDuration;
	}
}

void Crowd::Update(float deltaTime, Clip& mClip, unsigned int texWidth)
{
	bool looping = mClip.GetLooping();
	float start = mClip.GetStartTime();
	float end = mClip.GetEndTime();
	float duration = mClip.GetDuration();

	UpdatePlaybackTimes(deltaTime, looping, start, end);
	UpdateFrameIndices(start, duration, texWidth);
	UpdateInterpolationTimes(start, duration, texWidth);
}

void Crowd::SetUniforms(Shader* shader)
{
	Uniform<vec3>::Set(shader->GetUniform("model_pos"), mPositions);
	Uniform<quat>::Set(shader->GetUniform("model_rot"), mRotations);
	Uniform<vec3>::Set(shader->GetUniform("model_scl"), mScales);
	Uniform<ivec2>::Set(shader->GetUniform("frames"), mFrames);
	Uniform<float>::Set(shader->GetUniform("time"), mTimes);
}

void Crowd::RandomizeTimes(Clip& clip)
{
	float start = clip.GetStartTime();
	float duration = clip.GetDuration();

	unsigned int size = (unsigned int)mCurrentPlayTimes.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		float random = (float)rand() / (float)RAND_MAX;
		mCurrentPlayTimes[i] = random * duration + start;
	}
}

void Crowd::RandomizePositions(std::vector<vec3>& existing, const vec3& min, const vec3& max, float radius) {
	std::vector<vec3> positions;
	unsigned int size = (unsigned int)mCurrentPlayTimes.size();
	positions.reserve(size);
	vec3 delta = max - min;

	unsigned int breakLoop = 0;

	while (positions.size() < size) {
		if (breakLoop >= 2000) {
			break;
		}
		vec3 random((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
		vec3 point = min + delta * random;

		bool valid = true;
		for (unsigned int i = 0, iSize = (unsigned int)existing.size(); i < iSize; ++i) {
			if ((existing[i] - point).LenSq() < radius * radius) {
				valid = false;
				breakLoop += 1;
				break;
			}
		}

		if (valid) {
			breakLoop = 0;
			positions.push_back(point);
			existing.push_back(point);
		}
	}

	if (positions.size() != size) {
		Resize((unsigned int)positions.size());
	}

	memcpy(mPositions[0].v, positions[0].v, sizeof(float) * positions.size() * 3);
}