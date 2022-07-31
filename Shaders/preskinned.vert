#version 460 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 position;
in vec3 normal;
in vec2 texCoord;
in vec4 weights;
in ivec4 joints; //影响该顶点的关节数组

out vec3 norm;
out vec3 fragPos;
out vec2 uv;

uniform mat4 MatrixPalette[120];

void main() 
{
	mat4 skin = MatrixPalette[joints.x] * weights.x +
	MatrixPalette[joints.y] * weights.y +
	MatrixPalette[joints.z] * weights.z +
	MatrixPalette[joints.w] * weights.w;
	gl_Position = projection * view * model * skin * vec4(position, 1.0);

	fragPos = vec3(model * skin * vec4(position, 1.0));
	norm = vec3(model * skin * vec4(normal, 0.0f));
	uv = texCoord;
}