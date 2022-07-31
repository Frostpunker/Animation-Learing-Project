#pragma once
#include"Track.h"
#include"Transform.h"
template<typename VTRACK, typename QTRACK>
class TTransformTrack
{
protected:
	unsigned int mId;
	VTRACK mTranslate;
	QTRACK mRotation;
	VTRACK mScale;
public:
	TTransformTrack() : mId(0) {}
	unsigned int GetId() { return mId; }
	void SetId(unsigned int id) { mId = id; }
	VTRACK& GetTranslateTrack() { return mTranslate; }
	QTRACK& GetRotationTrack() { return mRotation; }
	VTRACK& GetScaleTrack() { return mScale; }
	float GetStartTime();
	float GetEndTime();
	bool IsValid();
	transform Sample(const transform& ref, float time, bool looping);
};

typedef TTransformTrack<VectorTrack, QuaternionTrack> TransformTrack;
typedef TTransformTrack<FastVectorTrack, FastQuaternionTrack> FastTransformTrack;

FastTransformTrack OptimizeTransformTrack(TransformTrack& input);
