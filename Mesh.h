#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include"Attribute.h"
#include"IndexBuffer.h"
#include"Skeleton.h"
#include"Pose.h"
#include"Matrix.h"
#include"Draw.h"

class Mesh
{
	//供CPU使用的数据
protected:
	std::vector<vec3> mPosition;
	std::vector<vec3> mNormal;
	std::vector<vec2> mTexCoord;
	std::vector<vec4> mWeights;
	std::vector<ivec4> mInfluences;
	std::vector<unsigned int> mIndices;

	//供GPU使用的数据
protected:
	Attribute<vec3>* mPosAttrib;
	Attribute<vec3>* mNormAttrib;
	Attribute<vec2>* mUvAttrib;
	Attribute<vec4>* mWeightAttrib;
	Attribute<ivec4>* mInfluenceAttrib;
	IndexBuffer* mIndexBuffer;

protected:
	std::vector<vec3> mSkinnedPosition;
	std::vector<vec3> mSkinnedNormal;
	
	//矩阵调色板
	//它由所有关节的蒙皮矩阵组成
	std::vector<mat4> mPosePalette;

public:
	Mesh();
	Mesh(const Mesh&);
	Mesh& operator=(const Mesh&);
	~Mesh();

	std::vector<vec3>& GetPosition() { return mPosition; }
	std::vector<vec3>& GetNormal() { return mNormal; }
	std::vector<vec2>& GetTexCoord() { return mTexCoord; }
	std::vector<vec4>& GetWeights() { return mWeights; }
	std::vector<ivec4>& GetInfluences() { return mInfluences; }
	std::vector<unsigned int>& GetIndices() { return mIndices; }

	//直接用SQT进行蒙皮
	void CPUSkinTrans(std::vector<mat4>& animatedPose);
	//void CPUSkinTrans(Skeleton& skeleton, Pose& pose);
	//利用矩阵调色板进行蒙皮
	void CPUSkinMat(Skeleton& skeleton, Pose& pose);
	void UpdateOpenGLBuffers();
	void Bind(int position, int normal, int texCoord,
		int weight, int influence);
	void Draw();
	void DrawInstanced(unsigned int numInstances);
	void UnBind(int position, int normal, int texCoord,
		int weight, int influence);
	
	//生成矩阵调色板
	//void GenMatrixPalette(Skeleton& skeleton, Pose& pose);

};
