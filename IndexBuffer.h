#pragma once
#include<vector>
#include"glad.h"

class IndexBuffer {
public:
	unsigned int mHandle;
	unsigned int mCount;
private:
	IndexBuffer(const IndexBuffer& other) = delete;
	IndexBuffer& operator=(const IndexBuffer& other) = delete;
public:
	IndexBuffer()
	{
		glGenBuffers(1, &mHandle);
		mCount = 0;
	}

	~IndexBuffer()
	{
		glDeleteBuffers(1, &mHandle);
	}

	void Set(unsigned int* rr, unsigned int len);
	void Set(std::vector<unsigned int>& input);
	unsigned int Count() { return mCount; }
	unsigned int GetHandle() { return mHandle; }
};
