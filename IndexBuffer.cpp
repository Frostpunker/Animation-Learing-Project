#include"IndexBuffer.h"

void IndexBuffer::Set(unsigned int* inputArray, unsigned int arrayLengt) 
{
	mCount = arrayLengt;
	unsigned int size = sizeof(unsigned int);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * mCount,
		inputArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Set(std::vector<unsigned int>& input) 
{
	Set(&input[0], (unsigned int)input.size());
}
