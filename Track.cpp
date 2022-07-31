#include"Track.h"
template Track<float, 1>;
template Track<vec3, 3>;
template Track<quat, 4>;


template FastTrack<float, 1>;
template FastTrack<vec3, 3>;
template FastTrack<quat, 4>;

namespace TrackHelpers
{
	inline float Interpolate(float a, float b, float t)
	{
		return a + (b - a) * t;
	}

	inline vec3 Interpolate(const vec3& a, const vec3& b, float t)
	{
		return Lerp(a, b, t);
	}

	inline quat Interpolate(const quat& a, const quat& b, float t)
	{
		quat result = mix(a, b, t);
		if (dot(a, b) < 0)
		{ // Neighborhood
			result = mix(a, -b, t);
		}
		return result.GetNormalized(); //NLerp
	}

	//标量和普通向量不需要调整
	inline float AdjustHermiteResult(float f)
	{
		return f;
	}

	inline vec3 AdjustHermiteResult(const vec3& v)
	{
		return v;
	}
	//四元数Hermite插值时需要归一化
	inline quat AdjustHermiteResult(const quat& q)
	{
		return q.GetNormalized();
	}

	inline void Neighborhood(const float& a, float& b) {}
	inline void Neighborhood(const vec3& a, vec3& b) {}
	
	inline void Neighborhood(const quat& a, quat& b) 
	{
		if (dot(a, b) < 0) 
		{
			b = -b;
		}
	}
};
//TrackHelpers end

//根据插值类型进行插值
template<typename T, size_t N>
T Track<T, N>::Sample(float time, bool looping) 
{
	if (mInterpolation == Interpolation::Constant) 
	{
		return SampleConstant(time, looping);
	}
	else if (mInterpolation == Interpolation::Linear) 
	{
		return SampleLinear(time, looping);
	}
		return SampleCubic_Her(time, looping);
}

//Hermite插值
template<typename T, size_t N>
T Track<T, N>::Hermite(float t, const T& p1, const T& s1, const T& _p2, const T& s2) 
{
	float tt = t * t;
	float ttt = tt * t;

	T p2 = _p2;
	TrackHelpers::Neighborhood(p1, p2);

	float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
	float h2 = -2.0f * ttt + 3.0f * tt;
	float h3 = ttt - 2.0f * tt + t;
	float h4 = ttt - tt;

	T result = p1 * h1 + p2 * h2 + s1 * h3 + s2 * h4;
	return TrackHelpers::AdjustHermiteResult(result);
}

//Bezier插值
template<typename T, size_t N>
T Track<T, N>::Bezier(float t, const T& p1, const T& c1, const T& _p2, const T& c2)
{
	using Bzi = ::Bezier<T>;
	T p2 = _p2;
	TrackHelpers::Neighborhood(p1, p2);
	Bzi bzi(p1, c1, p2, c2);
	Spline<Bzi> b(bzi);
	T result = b.Ierp(t);
	return TrackHelpers::AdjustHermiteResult(result);
}

template<typename T, size_t N>
int Track<T, N>::FrameIndex(float time, bool looping)
{
	size_t size = (size_t)mFrames.size();
	if (size <= 1) 
	{
		return -1;
	}
	
	if (looping) 
	{
		float startTime = mFrames[0].mTime;
		float endTime = mFrames[size - 1].mTime;
		float duration = endTime - startTime;
		time = fmodf(time - startTime, endTime - startTime);
		if (time < 0.0f) 
		{
			time += endTime - startTime;
		}
		time = time + startTime;

	}
	else 
	{
		if (time <= mFrames[0].mTime) 
		{
			return 0;
		}
		if (time >= mFrames[size - 2].mTime) 
		{
			return (int)size - 2;
		}
	}

	for (int i = (int)size - 1; i >= 0; --i) 
	{
		if (time >= mFrames[i].mTime) 
		{
			return i;
		}
	}
}

//将时间规范化到一个Track内
template<typename T, size_t N>
float Track<T, N>::AdjustTimeToFitTrack(float time, bool looping)
{
	size_t size = (size_t)mFrames.size();
	if (size <= 1) 
	{
		return 0.0f;
	}
	float startTime = mFrames[0].mTime;
	float endTime = mFrames[size - 1].mTime;
	float duration = endTime - startTime;
	if (duration <= 0.0f) 
	{
		return 0.0f;
	}

	if (looping) 
	{
		time = fmodf(time - startTime, endTime - startTime);
		if (time < 0.0f) 
		{
			time += endTime - startTime;
		}
		time = time + startTime;
	}
	else 
	{
		if (time <= mFrames[0].mTime) 
		{
			time = startTime;
		}
		if (time >= mFrames[size - 1].mTime) 
		{
			time = endTime;
		}
	}
	return time;
}

template<> float Track<float, 1>::Cast(float* value) 
{
	return value[0];
}

template<> vec3 Track<vec3, 3>::Cast(float* value) 
{
	return vec3(value[0], value[1], value[2]);
}

template<> quat Track<quat, 4>::Cast(float* value) 
{
	quat r = quat(value[0], value[1], value[2], value[3]);
	return r.GetNormalized();
}

template<typename T, size_t N>
T Track<T, N>::SampleConstant(float t, bool loop) 
{
	int frame = FrameIndex(t, loop);
	if (frame < 0 || frame >= (int)mFrames.size()) 
	{
		return T();
	}
	return Cast(&mFrames[frame].mValue[0]);
}

template<typename T, size_t N>
T Track<T, N>::SampleLinear(float time, bool looping) 
{
	int thisFrame = FrameIndex(time, looping);
	//边界检查    
	if (thisFrame < 0 || thisFrame >= mFrames.size() - 1) 
	{
		return T();
	}
	int nextFrame = thisFrame + 1;
	float trackTime = AdjustTimeToFitTrack(time, looping);
	float frameDelta = mFrames[nextFrame].mTime - mFrames[thisFrame].mTime;
	if (frameDelta <= 0.0f) 
	{
		return T();
	}
	float t = (trackTime - mFrames[thisFrame].mTime) / frameDelta;
	T start = Cast(&mFrames[thisFrame].mValue[0]);
	T end = Cast(&mFrames[nextFrame].mValue[0]);
	return TrackHelpers::Interpolate(start, end, t);
}

template<typename T, size_t N>
T Track<T, N>::SampleCubic_Her(float time, bool looping) 
{
	int thisFrame = FrameIndex(time, looping);
	if (thisFrame < 0 || thisFrame >= mFrames.size() - 1) 
	{
		return T();
	}
	int nextFrame = thisFrame + 1;
	float trackTime = AdjustTimeToFitTrack(time, looping);
	float frameDelta = mFrames[nextFrame].mTime - mFrames[thisFrame].mTime;
	if (frameDelta <= 0.0f) 
	{
		return T();
	}
	float t = (trackTime - mFrames[thisFrame].mTime) / frameDelta;
	size_t fltSize = sizeof(float);
	T point1 = Cast(&mFrames[thisFrame].mValue[0]);
	T slope1;// = mFrames[thisFrame].mOut * frameDelta;
	memcpy(&slope1, mFrames[thisFrame].mOut, N * fltSize);
	slope1 = slope1 * frameDelta;
	T point2 = Cast(&mFrames[nextFrame].mValue[0]);
	T slope2;// = mFrames[nextFrame].mIn[0] * frameDelta;
	memcpy(&slope2, mFrames[nextFrame].mIn, N * fltSize);
	slope2 = slope2 * frameDelta;
	return Hermite(t, point1, slope1, point2, slope2);
}
//end Track class


//start FastTrack class
//将所有采样点规范化到一个Frame的索引上
template<typename T, size_t N>
void FastTrack<T, N>::UpdateIndexLookupTable()
{
	//保证一个Track至少有两帧以上
	int numFrames = (int)this->mFrames.size();
	if (numFrames <= 1)
	{
		return;
	}

	float duration = this->GetEndTime() - this->GetStartTime();
	//60FPS
	unsigned int numSamples = 60 + (unsigned int)(duration * 60.0f);
	mSampledFrames.resize(numSamples);

	for (unsigned int i = 0; i < numSamples; ++i)
	{
		//计算每个采样点的对应时间点
		float t = (float)i / (float)(numSamples - 1);
		float time = t * duration + this->GetStartTime();

		unsigned int frameIndex = 0;
		for (int j = numFrames - 1; j >= 0; --j)
		{
			if (time >= this->mFrames[j].mTime)
			{
				frameIndex = (unsigned int)j;
				if ((int)frameIndex >= numFrames - 2)
				{
					//返回倒数第二帧之后结束
					frameIndex = numFrames - 2;
				}
				break;
			}
		}
		mSampledFrames[i] = frameIndex;
	}
}

template<typename T, size_t N>
int FastTrack<T, N>::FrameIndex(float time, bool loop)
{
	std::vector<Frame<N>>& frames = this->mFrames;
	unsigned int size = (unsigned int)frames.size();
	if (size <= 1)
	{
		return -1;
	}

	//将time规范化到一个Track中
	if (loop) {
		float startTime = frames[0].mTime;
		float endTime = frames[size - 1].mTime;
		float duration = endTime - startTime;
		time = fmodf(time - startTime, endTime - startTime);
		if (time < 0.0f) {
			time += endTime - startTime;
		}
		time = time + startTime;
	}
	else
	{
		if (time <= frames[0].mTime)
		{
			return 0;
		}
		//返回倒数第二帧
		if (time >= frames[size - 2].mTime)
		{
			return (int)size - 2;
		}
	}

	//根据time定位采样点，依照sampledframes数组中的值直接返回该采样点的FrameIndex
	float duration = this->GetEndTime() - this->GetStartTime();
	unsigned int numSamples = 60 + (unsigned int)(duration * 60.0f);
	float t = time / duration;
	unsigned int index = (unsigned int)(t * (float)numSamples);
	if (index >= mSampledFrames.size())
	{
		return -1;
	}
	return (int)mSampledFrames[index];
}

template FastTrack<float, 1> OptimizeTrack(Track<float, 1>& input);
template FastTrack<vec3, 3> OptimizeTrack(Track<vec3, 3>& input);
template FastTrack<quat, 4> OptimizeTrack(Track<quat, 4>& input);

template<typename T, size_t N>
FastTrack<T, N> OptimizeTrack(Track<T, N>& input)
{
	FastTrack<T, N> result;
	result.SetInterpolation(input.GetInterpolation());
	unsigned int size = input.Size();
	result.Resize(size);
	for (unsigned int i = 0; i < size; ++i) 
	{
		result[i] = input[i];
	}
	result.UpdateIndexLookupTable();
	return result;
}


