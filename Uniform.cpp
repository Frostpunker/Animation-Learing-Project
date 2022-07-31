#include"Uniform.h"
#include"Vector2.h"
#include"Quat.h"
#include"glad.h"

template Uniform<int>;
template Uniform<ivec4>;
template Uniform<ivec2>;
template Uniform<float>;
template Uniform<vec2>;
template Uniform<vec3>;
template Uniform<vec4>;
template Uniform<quat>;
template Uniform<mat4>;
template Uniform<Dquat>;

//单个常量
template <typename T>
void Uniform<T>::Set(unsigned int slot, const T& value)
{
	Set(slot, (T*)&value, 1);
}

//一个vector
template <typename T>
void Uniform<T>::Set(unsigned int s, std::vector<T>& v)
{
	Set(s, &v[0], (unsigned int)v.size());
}

#define UNIFORM_IMPL(gl_func, tType, dType)\
template<> void Uniform<tType>::Set(unsigned int slot,\
 tType* data, unsigned int length) {\
 gl_func(slot, (GLsizei)length, (dType*)&data[0]); \
}

UNIFORM_IMPL(glUniform1iv, int, int)
UNIFORM_IMPL(glUniform4iv, ivec4, int)
UNIFORM_IMPL(glUniform2iv, ivec2, int)
UNIFORM_IMPL(glUniform1fv, float, float)
UNIFORM_IMPL(glUniform2fv, vec2, float)
UNIFORM_IMPL(glUniform3fv, vec3, float)
UNIFORM_IMPL(glUniform4fv, vec4, float)
UNIFORM_IMPL(glUniform4fv, quat, float)

//4x4浮点矩阵
template<> 
void Uniform<mat4>::Set(unsigned int slot, mat4* inputArray, unsigned int arrayLength)
{
	glUniformMatrix4fv(slot, (GLsizei)arrayLength, false, (float*)&inputArray[0]);
}

//用2x4浮点矩阵表示对偶四元数
template<>
void Uniform<Dquat>::Set(unsigned int slot,Dquat* inputArray, unsigned int arrayLength) 
{
	glUniformMatrix2x4fv(slot, arrayLength, false, inputArray[0].v);
}