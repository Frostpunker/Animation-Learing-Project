#include"Intersection.h"

bool RaycastTriangle(const Ray& ray, const Triangle& triangle, vec3& hitPoint) 
{
	const float EPSILON = 0.0000001f;
	vec3 vertex0 = triangle.v0;
	vec3 vertex1 = triangle.v1;
	vec3 vertex2 = triangle.v2;
	vec3 edge1, edge2, h, s, q;
	vec3 rayVector = ray.direction;
	vec3 rayOrigin = ray.origin;
	float a, f;
	//�����ε��������꣺u + v + w = 1
	//w����Ҫ�洢
	float u, v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = cross(rayVector, edge2);
	//e1*(D x e2)
	a = dot(edge1, h);
	//Rayƽ�������������򷵻�False
	if (a > -EPSILON && a < EPSILON)
		return false;

	f = 1.0f / a;
	s = rayOrigin - vertex0;
	//v0������ԭ��������v0��v1������h�ϵ�ͶӰ֮��
	u = f * dot(s, h);
	//u��0~1֮���ʾ��������������
	if (u < 0.0f || u > 1.0f)
		return false;

	q = cross(s, edge1);
	v = f * dot(rayVector, q);
	//v��wС��0��ͨ��
	if (v < 0.0f || u + v > 1.0f)
		return false;

	float t = f * dot(edge2, q);
	if (t > EPSILON)
	{
		hitPoint = rayOrigin + rayVector * t;
		return true;
	}

	return false;
}

std::vector<Triangle> MeshToTriangles(Mesh& mesh) {
	//���ݶ��㹹��������
	std::vector<Triangle> result;
	//��������
	std::vector<vec3> vertices = mesh.GetPosition();
	std::vector<unsigned int> indices = mesh.GetIndices();
	//���û������
	if (indices.size() == 0) {
		unsigned int numVertices = (unsigned int)vertices.size();
		//ÿ��������Ϊһ��
		for (unsigned int i = 0; i < numVertices; i += 3) {
			result.push_back(Triangle(
				vertices[i + 0],
				vertices[i + 1],
				vertices[i + 2]
			));
		}
	}
	//����������������������
	else {
		unsigned int numIndices = (unsigned int)indices.size();
		for (unsigned int i = 0; i < numIndices; i += 3) {
			result.push_back(Triangle(
				vertices[indices[i + 0]],
				vertices[indices[i + 1]],
				vertices[indices[i + 2]]
			));
		}
	}
	return result;
}

std::vector<Triangle> MeshesToTriangles(std::vector<Mesh>& meshes) {
	std::vector<Triangle> result;
	unsigned int numMeshes = (unsigned int)meshes.size();
	for (unsigned int j = 0; j < numMeshes; ++j) {
		Mesh& mesh = meshes[j];
		std::vector<vec3> vertices = mesh.GetPosition();
		std::vector<unsigned int> indices = mesh.GetIndices();
		if (indices.size() == 0) {
			unsigned int numVertices = (unsigned int)vertices.size();
			for (unsigned int i = 0; i < numVertices; i += 3) {
				result.push_back(Triangle(
					vertices[i + 0],
					vertices[i + 1],
					vertices[i + 2]
				));
			}
		}
		else {
			unsigned int numIndices = (unsigned int)indices.size();
			for (unsigned int i = 0; i < numIndices; i += 3) {
				result.push_back(Triangle(
					vertices[indices[i + 0]],
					vertices[indices[i + 1]],
					vertices[indices[i + 2]]
				));
			}
		}
	}
	return result;
}