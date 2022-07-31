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
	//��CPUʹ�õ�����
protected:
	std::vector<vec3> mPosition;
	std::vector<vec3> mNormal;
	std::vector<vec2> mTexCoord;
	std::vector<vec4> mWeights;
	std::vector<ivec4> mInfluences;
	std::vector<unsigned int> mIndices;

	//��GPUʹ�õ�����
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
	
	//�����ɫ��
	//�������йؽڵ���Ƥ�������
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

	//ֱ����SQT������Ƥ
	void CPUSkinTrans(std::vector<mat4>& animatedPose);
	//void CPUSkinTrans(Skeleton& skeleton, Pose& pose);
	//���þ����ɫ�������Ƥ
	void CPUSkinMat(Skeleton& skeleton, Pose& pose);
	void UpdateOpenGLBuffers();
	void Bind(int position, int normal, int texCoord,
		int weight, int influence);
	void Draw();
	void DrawInstanced(unsigned int numInstances);
	void UnBind(int position, int normal, int texCoord,
		int weight, int influence);
	
	//���ɾ����ɫ��
	//void GenMatrixPalette(Skeleton& skeleton, Pose& pose);

};
