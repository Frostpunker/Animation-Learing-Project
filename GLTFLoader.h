#pragma once
#include"cgltf.h"
#include"Clip.h"
#include"Mesh.h"
#include"Skeleton.h"

namespace GLTFHelpers
{
	transform GetLocalTransform(cgltf_node& n);

	int GetNodeIndex(cgltf_node* target,
		cgltf_node* allNodes, unsigned int numNodes);

	void GetScalarValues(std::vector<float>& out,
		unsigned int compCount,
		const cgltf_accessor& inAccessor);

	template<typename T, size_t N>
	void TrackFromChannel(Track<T, N>& result,
		const cgltf_animation_channel& channel);

	void MeshFromAttribute(Mesh& outMesh,
		cgltf_attribute& attribute,
		cgltf_skin* skin, cgltf_node* nodes,
		unsigned int nodeCount);

	void StaticMeshFromAttribute(Mesh& outMesh, cgltf_attribute& attribute);

}

std::vector<Clip> LoadAnimationClips(cgltf_data* data);

cgltf_data* LoadGLTFFile(const char* path);
void FreeGLTFFile(cgltf_data* data);
Skeleton LoadSkeleton(cgltf_data* data);
std::vector<Mesh> LoadMeshes(cgltf_data* data);
std::vector<Mesh> LoadStaticMeshes(cgltf_data* data);