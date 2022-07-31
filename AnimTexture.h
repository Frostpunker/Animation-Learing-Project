/*动画纹理的数据结构*/
#pragma once
#include"Vector3.h"
#include"Vector4.h"
#include"Quat.h"

class AnimTexture
{
protected:
	float* mData; //数据
	unsigned int mSize; //纹理大小
	unsigned int mHandle;//OpenGL句柄
public:
	AnimTexture();
	AnimTexture(const AnimTexture&);
	AnimTexture& operator=(const AnimTexture&);
	~AnimTexture();
	void Load(const char* path);
	void Save(const char* path);
	void UploadTextureDataToGPU();

	unsigned int Size();
	void Resize(unsigned int newSize);
	float* GetData();

	void Set(unsigned int uniformIndex, unsigned int textureIndex);
	void UnSet(unsigned int textureIndex);
	unsigned int GetHandle();

	void SetTexel(unsigned int x, unsigned int y, const vec3& v);
	void SetTexel(unsigned int x, unsigned int y, const quat& q);
	vec4 GetTexel(unsigned int x, unsigned int y);
};

