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
	//读取一个shader源文件并返回一个OpenGL句柄
	unsigned int CompileVertexShader(const std::string& vertex);
	unsigned int CompileFragmentShader(const std::string& fragment);
	//链接shader文件并生成一个shader程序
	bool LinkShaders(unsigned int vertex,unsigned int fragment);

	//添加数据
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

	//加载shader并编译
	void Load(const std::string& vertex,const std::string& fragment);

	void Bind();
	void UnBind();
	unsigned int GetAttribute(const std::string& name);
	unsigned int GetUniform(const std::string& name);
	unsigned int GetHandle();

};