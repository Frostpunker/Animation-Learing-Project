#pragma once
#include "Shader.h"
#include "Attribute.h"
#include "Vector3.h"
#include "Matrix.h"
#include <vector>
#include "Pose.h"
#include "CCDSolver.h"
#include "FABRIKSolver.h"

enum class DebugDrawMode {
	Lines, Loop, Strip, Points
};

class DebugDraw {
protected:
	std::vector<vec3> mPoints;
	Attribute<vec3>* mAttribs;
	Shader* mShader;
private:
	DebugDraw(const DebugDraw&) = delete;
	DebugDraw& operator=(const DebugDraw&) = delete;
public:
	DebugDraw();
	DebugDraw(unsigned int size);
	~DebugDraw();

	unsigned int Size();
	void Resize(unsigned int newSize);
	vec3& operator[](unsigned int index);
	void Push(const vec3& v);

	//将mPoints设置为以当前关节和其父关节全局变换为一组的向量
	void FromPose(Pose& pose);
	void LinesFromIKSolver(CCDSolver& solver);
	void PointsFromIKSolver(CCDSolver& solver);
	void LinesFromIKSolver(FABRIKSolver& solver);
	void PointsFromIKSolver(FABRIKSolver& solver);

	void UpdateOpenGLBuffers();
	void Draw(DebugDrawMode mode, const vec3& color, const mat4& mvp);
};

