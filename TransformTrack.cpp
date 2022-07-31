#include"TransformTrack.h"

template TTransformTrack<VectorTrack, QuaternionTrack>;
template TTransformTrack<FastVectorTrack, FastQuaternionTrack>;

template <typename VTRACK, typename QTRACK>
bool TTransformTrack<VTRACK, QTRACK>::IsValid() 
{
	return mTranslate.Size() > 1 ||
		mRotation.Size() > 1 ||
		mScale.Size() > 1;
}

//返回可用分量中的最小时间
template <typename VTRACK, typename QTRACK>
float TTransformTrack<VTRACK, QTRACK>::GetStartTime()
{
	float result = 0.0f;
	bool isSet = false;
	if (mTranslate.Size() > 1) 
	{
		result = mTranslate.GetStartTime();
		isSet = true;
	}
	if (mRotation.Size() > 1) 
	{
		float rotationStart = mRotation.GetStartTime();
		if (rotationStart < result || !isSet) 
		{
			result = rotationStart;
			isSet = true;
		}
	}
	if (mScale.Size() > 1)
	{
		float scaleStart = mScale.GetStartTime();
		if (scaleStart < result || !isSet) 
		{
			result = scaleStart;
			isSet = true;
		}
	}
	return result;
}

//返回可用分量中的最小时间
template <typename VTRACK, typename QTRACK>
float TTransformTrack<VTRACK, QTRACK>::GetEndTime()
{
	float result = 0.0f;
	bool isSet = false;
	if (mTranslate.Size() > 1) 
	{
		result = mTranslate.GetEndTime();
		isSet = true;
	}
	if (mRotation.Size() > 1) 
	{
		float rotationEnd = mRotation.GetEndTime();
		if (rotationEnd > result || !isSet) 
		{
			result = rotationEnd;
			isSet = true;
		}
	}
	if (mScale.Size() > 1) 
	{
		float scaleEnd = mScale.GetEndTime();
		if (scaleEnd > result || !isSet) 
		{
			result = scaleEnd;
			isSet = true;
		}
	}
	return result;
}

template <typename VTRACK, typename QTRACK>
transform TTransformTrack<VTRACK, QTRACK>::Sample(const transform& ref, float time, 
	bool loop)
{
	transform result = ref; 
	if (mTranslate.Size() > 1) 
	{ 
		result.translate = mTranslate.Sample(time, loop);
	}
	if (mRotation.Size() > 1) 
	{ 
		result.rotation = mRotation.Sample(time, loop);
	}
	if (mScale.Size() > 1) 
	{
		result.scale = mScale.Sample(time, loop);
	}
	return result;
}


FastTransformTrack OptimizeTransformTrack(TransformTrack& input) 
{
	FastTransformTrack result;
	result.SetId(input.GetId());
	result.GetTranslateTrack() = OptimizeTrack<vec3, 3>(
		input.GetTranslateTrack());
	result.GetRotationTrack() = OptimizeTrack<quat, 4>(
		input.GetRotationTrack());
	result.GetScaleTrack() = OptimizeTrack<vec3, 3>(
		input.GetScaleTrack());
	return result;
}