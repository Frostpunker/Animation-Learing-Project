#pragma once
#include "glad.h"
#include <fstream>
#include <sstream>
#include<iostream>
#include<map>
class Shader
{
private:
	unsigned int mHandle;

	std::map<std::string, unsigned int> mAttributes;
	std::map<std::string, unsigned int> mUniforms;

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	std::string ReadFile(const std::string& path);
	//��ȡһ��shaderԴ�ļ�������һ��OpenGL���
	unsigned int CompileVertexShader(const std::string& vertex);
	unsigned int CompileFragmentShader(const std::string& fragment);
	//����shader�ļ�������һ��shader����
	bool LinkShaders(unsigned int vertex,unsigned int fragment);

	//�������
	void PopulateAttributes();
	void PopulateUniforms();

public:
	Shader() 
	{
		mHandle = glCreateProgram();
	}

	Shader(const std::string& vertex, const std::string& fragment)
	{
		mHandle = glCreateProgram();
		Load(vertex, fragment);
	}

	~Shader() {
		glDeleteProgram(mHandle);
	}

	//����shader������
	void Load(const std::string& vertex,const std::string& fragment);

	void Bind();
	void UnBind();
	unsigned int GetAttribute(const std::string& name);
	unsigned int GetUniform(const std::string& name);
	unsigned int GetHandle();

};