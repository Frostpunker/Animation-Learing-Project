#include"Texture.h"

void Texture::Load(const char* path) 
{
	glBindTexture(GL_TEXTURE_2D, mHandle);
	int width, height, channels;
	//通过图片路径加载其高度、宽度、颜色通道
	unsigned char* data = stbi_load(path, &width, &height, &channels, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//生成纹理对应的MipMap
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	mWidth = width;
	mHeight = height;
	mChannels = channels;
}

void Texture::Set(unsigned int uniformIndex, unsigned int textureIndex) 
{
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, mHandle);
	glUniform1i(uniformIndex, textureIndex);
}

void Texture::UnSet(unsigned int textureIndex) 
{
	glActiveTexture(GL_TEXTURE0 + textureIndex);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
}