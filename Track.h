#pragma once
#include"Frame.h"

enum class Interpolation
{
	Constant,
	Linear,
	Cubic
};

template<typename T, size_t N>
class Track
{
protected:
	
	std::vector<Frame<N>> mFrames;
	Interpolation mInterpolation;

public:
	Track()
	{
		mInterpolation = Interpolation::Linear;
	}

	void Resize(size_t size)
	{
		mFrames.resize(size);
	}

	size_t Size()
	{
		return mFrames.size();
	}

	Interpolation GetInterpolation()
	{
		return mInterpolation;
	}

	void SetInterpolation(Interpolation interp)
	{
		mInterpolation = interp;
	}
	
	float GetStartTime() 
	{
		return mFrames[0].mTime;
	}
	
	float GetEndTime()
	{
		return mFrames[mFrames.size() - 1].mTime;
	}
		
	Frame<N>& operator[](size_t index)
	{
		return mFrames[index];
	}
	
	//根据插值类型调用不同的采样函数
	T Sample(float time, bool looping);

protected:
	T SampleConstant(float time, bool looping);
	T SampleLinear(float time, bool looping);
	T SampleCubic_Her(float time, bool looping);
	//T SampleCubic_Bzi(float time, bool looping) {};
	T Hermite(float time, const T& p1, const T& s1, const T& p2, const T& s2);
	T Bezier(float time, const T& p1, const T& c1, const T& p2, const T& c2);
	virtual int FrameIndex(float time, bool looping);
	float AdjustTimeToFitTrack(float t, bool loop);
	T Cast(float* value); // Will be specialized
};

typedef Track<float, 1> ScalarTrack;
typedef Track<vec3, 3> VectorTrack;
typedef Track<quat, 4> QuaternionTrack;

//针对Track的优化方案
template<typename T, size_t N>
class FastTrack : public Track<T, N>
{
protected:
	std::vector<unsigned int> mSampledFrames;
	virtual int FrameIndex(float time, bool looping) override;
public:
	void UpdateIndexLookupTable();
};

typedef FastTrack<float, 1> FastScalarTrack;
typedef FastTrack<vec3, 3> FastVectorTrack;
typedef FastTrack<quat, 4> FastQuaternionTrack;

//用于将原Track class转换到FastTrack class
template<typename T, size_t N>
FastTrack<T, N> OptimizeTrack(Track<T, N>& input);