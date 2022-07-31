#pragma once
#include"glad.h"
#include"stb_image.h"
class Texture 
{
protected:
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mChannels;
	unsigned int mHandle;

private:
	Texture(const Texture& other) = delete;
	Texture& operator=(const Texture& other) = delete;

public:
	Texture() :mWidth(0), mHeight(0), mChannels(0)
	{
		glGenTextures(1, &mHandle);
	}

	Texture(const char* path)
	{
		glGenTextures(1, &mHandle);
		Load(path);
	}

	~Texture()
	{
		glDeleteTextures(1, &mHandle);
	}

	void Load(const char* path);
	void Set(unsigned int uniform, unsigned int texIndex);
	void UnSet(unsigned int textureIndex);
	unsigned int GetHandle() { return mHandle; }
};
