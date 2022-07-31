#include"GLTFLoader.h"
#include"Skeleton.h"
namespace GLTFHelpers
{
	transform GetLocalTransform(cgltf_node& n)
	{
		transform result;
		if (n.has_matrix) 
		{
			mat4 mat(&n.matrix[0]);
			result = mat4ToTransform(mat);
		}
		if (n.has_translation) 
		{
			result.translate = vec3(n.translation[0],
				n.translation[1], n.translation[2]);
		}
		if (n.has_rotation) 
		{
			result.rotation = quat(n.rotation[0], 
				n.rotation[1], n.rotation[2], n.rotation[3]);
		}
		if (n.has_scale) 
		{
			result.scale = vec3(n.scale[0], n.scale[1], n.scale[2]);
		}
		return result;
	}

	int GetNodeIndex(cgltf_node* target,
		cgltf_node* allNodes, unsigned int numNodes) 
	{
		if (target == 0) 
		{
			return -1;
		}
		for (unsigned int i = 0; i < numNodes; ++i) 
		{
			if (target == &allNodes[i]) 
			{
				return (int)i;
			}
		}
		return -1;
	}

	void GetScalarValues(std::vector<float>& out,
		unsigned int compCount,
		const cgltf_accessor& inAccessor) 
	{
		out.resize(inAccessor.count * compCount);
		for (cgltf_size i = 0; i < inAccessor.count; ++i) 
		{
			cgltf_accessor_read_float(&inAccessor, i,
				&out[i * compCount],
				compCount);
		}
	}

	template<typename T, size_t N>
	void TrackFromChannel(Track<T, N>& result,
		const cgltf_animation_channel& channel)
	{
		cgltf_animation_sampler& sampler = *channel.sampler;
		//匹配插值类型
		Interpolation interpolation = Interpolation::Constant;
		if (sampler.interpolation == cgltf_interpolation_type_linear) 
		{
			interpolation = Interpolation::Linear;
		}
		else if (sampler.interpolation == cgltf_interpolation_type_cubic_spline) 
		{
			interpolation = Interpolation::Cubic;
		}
		
		bool isSamplerCubic = interpolation == Interpolation::Cubic;
		result.SetInterpolation(interpolation);


		std::vector<float> time; // times
		GetScalarValues(time, 1, *sampler.input);
		std::vector<float> val; // values
		GetScalarValues(val, N, *sampler.output);
		size_t numFrames = sampler.input->count;
		size_t compCount = val.size() / time.size();
		result.Resize(numFrames);

		for (unsigned int i = 0; i < numFrames; ++i) 
		{
			int baseIndex = i * compCount;
			Frame<N>& frame = result[i];
			int offset = 0;
			frame.mTime = time[i];
			for (int comp = 0; comp < N; ++comp) 
			{
				frame.mIn[comp] = isSamplerCubic ?
					val[baseIndex + offset++] : 0.0f;
			}
			for (int comp = 0; comp < N; ++comp) 
			{
				frame.mValue[comp] = val[baseIndex + offset++];
			}
			for (int comp = 0; comp < N; ++comp) 
			{
				frame.mOut[comp] = isSamplerCubic ?
					val[baseIndex + offset++] : 0.0f;
			}
		}
	}

	void MeshFromAttribute(Mesh& outMesh,
		cgltf_attribute& attribute,
		cgltf_skin* skin, cgltf_node* nodes,
		unsigned int nodeCount)
	{
		cgltf_attribute_type attribType = attribute.type;
		cgltf_accessor& accessor = *attribute.data;
		unsigned int componentCount = 0;
		if (accessor.type == cgltf_type_vec2) 
		{
			componentCount = 2;
		}
		else if (accessor.type == cgltf_type_vec3) 
		{
			componentCount = 3;
		}
		else if (accessor.type == cgltf_type_vec4) 
		{
			componentCount = 4;
		}

		std::vector<float> values;
		GetScalarValues(values, componentCount, accessor);
		unsigned int acessorCount = accessor.count;
		std::vector<vec3>& positions = outMesh.GetPosition();
		std::vector<vec3>& normals = outMesh.GetNormal();
		std::vector<vec2>& texCoords = outMesh.GetTexCoord();
		std::vector<ivec4>& influences = outMesh.GetInfluences();
		std::vector<vec4>& weights = outMesh.GetWeights();

		for (unsigned int i = 0; i < acessorCount; ++i) 
		{
			int index = i * componentCount;
			switch (attribType) 
			{
			//position
			case cgltf_attribute_type_position:
				positions.push_back(vec3(values[index + 0],
					values[index + 1],
					values[index + 2]));
				break;
			//uv
			case cgltf_attribute_type_texcoord:
				texCoords.push_back(vec2(values[index + 0],
					values[index + 1]));
				break;
			//weights
			case cgltf_attribute_type_weights:
				weights.push_back(vec4(values[index + 0],
					values[index + 1],
					values[index + 2],
					values[index + 3]));
				break;
			//normal
			case cgltf_attribute_type_normal:
			{
				vec3 normal = vec3(values[index + 0],
					values[index + 1],
					values[index + 2]);
				if (normal.LenSq() < EPSILON) {
					normal = vec3(0, 1, 0);
				}
				normals.push_back(normal.GetNormalized());
			}
			break;
			case cgltf_attribute_type_joints:
			{
				// These indices are skin relative. This
				// function has no information about the
				// skin that is being parsed. Add +0.5f to
				// round, since we can't read integers
				ivec4 joints(
					(int)(values[index + 0] + 0.5f),
					(int)(values[index + 1] + 0.5f),
					(int)(values[index + 2] + 0.5f),
					(int)(values[index + 3] + 0.5f)
				);
				joints.x = GetNodeIndex(
					skin->joints[joints.x],
					nodes, nodeCount);
				joints.y = GetNodeIndex(
					skin->joints[joints.y],
					nodes, nodeCount);
				joints.z = GetNodeIndex(
					skin->joints[joints.z],
					nodes, nodeCount);
				joints.w = GetNodeIndex(skin->joints[joints.w],
					nodes, nodeCount);
				joints.x = std::max(0, joints.x);
				joints.y = std::max(0, joints.y);
				joints.z = std::max(0, joints.z);
				joints.w = std::max(0, joints.w);
				influences.push_back(joints);
			}
			break;
			}
		}
	}

	void StaticMeshFromAttribute(Mesh& outMesh, cgltf_attribute& attribute) 
	{
		cgltf_attribute_type attribType = attribute.type;
		cgltf_accessor& accessor = *attribute.data;

		unsigned int componentCount = 0;
		if (accessor.type == cgltf_type_vec2) 
		{
			componentCount = 2;
		}
		else if (accessor.type == cgltf_type_vec3)
		{
			componentCount = 3;
		}
		else if (accessor.type == cgltf_type_vec4) 
		{
			componentCount = 4;
		}
		else 
		{
			std::cout << "Unknown data type\n";
			return;
		}

		std::vector<float> values;
		GetScalarValues(values, componentCount, accessor);
		unsigned int acessorCount = (unsigned int)accessor.count;

		std::vector<vec3>& positions = outMesh.GetPosition();
		std::vector<vec3>& normals = outMesh.GetNormal();
		std::vector<vec2>& texCoords = outMesh.GetTexCoord();

		for (unsigned int i = 0; i < acessorCount; ++i) 
		{
			int index = i * componentCount;
			switch (attribType) 
			{
			case cgltf_attribute_type_position:
				positions.push_back(vec3(values[index + 0], values[index + 1], values[index + 2]));
				break;
			case cgltf_attribute_type_normal:
			{
				vec3 normal = vec3(values[index + 0], values[index + 1], values[index + 2]);
				if ((normal).LenSq() < 0.000001f) 
				{
					normal = vec3(0, 1, 0);
				}
				normals.push_back(normal.GetNormalized());
			}
			break;
			case cgltf_attribute_type_texcoord:
				texCoords.push_back(vec2(values[index + 0], values[index + 1]));
				break;
			}
		}
	}
}
//GLTFHelpers end
void FreeGLTFFile(cgltf_data* data) 
{
	if (data == 0) 
	{
		std::cout << "WARNING: Can't free null data\n";
	}
	else 
	{
		cgltf_free(data);
	}
}

cgltf_data* LoadGLTFFile(const char* path) 
{
	cgltf_options options;
	memset(&options, 0, sizeof(cgltf_options));
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, path, &data);
	if (result != cgltf_result_success)
	{
		std::cout << "Could not load input file: " << path << "\n";
		return 0;
	}
	result = cgltf_load_buffers(&options, data, path);
	if (result != cgltf_result_success) 
	{
		cgltf_free(data);
		std::cout << "Could not load buffers for: " << path << "\n";
		return 0;
	}
	result = cgltf_validate(data);
	if (result != cgltf_result_success) 
	{
		cgltf_free(data);
		std::cout << "Invalid gltf file: " << path << "\n";
		return 0;
	}
	return data;
}

Pose LoadRestPose(cgltf_data* data) 
{
	unsigned int boneCount = data->nodes_count;
	Pose result(boneCount);
	//遍历关节提取数据
	for (unsigned int i = 0; i < boneCount; ++i) 
	{
		cgltf_node* node = &(data->nodes[i]);
		//将文件中的矩阵数据转换为变换数据
		transform transform = GLTFHelpers::GetLocalTransform(data->nodes[i]);
		result.SetLocalTransform(i, transform);
		int parent = GLTFHelpers::GetNodeIndex(
				node->parent, data->nodes,
				boneCount);
		result.SetParent(i, parent);
	}
	return result;
}

//返回关节名称列表
std::vector<std::string> LoadJointNames(cgltf_data* data) 
{
	unsigned int boneCount = (unsigned int)data->nodes_count;
	std::vector<std::string> result(boneCount, "Not Set");
	for (unsigned int i = 0; i < boneCount; ++i) 
	{
		cgltf_node* node = &(data->nodes[i]);
		if (node->name == 0) 
		{
			result[i] = "EMPTY NODE";
		}
		else 
		{
			result[i] = node->name;
		}
	}
	return result;
}

std::vector<Clip> LoadAnimationClips(cgltf_data* data)
{
	unsigned int numClips = data->animations_count;
	unsigned int numNodes = data->nodes_count;
	std::vector<Clip> result;
	result.resize(numClips);

	for (unsigned int i = 0; i < numClips; ++i) 
	{
		result[i].SetName(data->animations[i].name);
		unsigned int numChannels = data->animations[i].channels_count;
		for (unsigned int j = 0; j < numChannels; ++j)
		{
			cgltf_animation_channel& channel = data->animations[i].channels[j];
			cgltf_node* target = channel.target_node;
			int nodeId = GLTFHelpers::GetNodeIndex(
				target, data->nodes, numNodes);
			//平移通道
			if (channel.target_path ==
				cgltf_animation_path_type_translation) 
			{
				VectorTrack& track =
					result[i][nodeId].GetTranslateTrack();
				GLTFHelpers::TrackFromChannel<vec3, 3>
					(track, channel);
			}
			//缩放通道
			else if (channel.target_path ==
				cgltf_animation_path_type_scale) {
				VectorTrack& track =
					result[i][nodeId].GetScaleTrack();
				GLTFHelpers::TrackFromChannel<vec3, 3>
					(track, channel);
			}
			//旋转通道
			else if (channel.target_path ==
				cgltf_animation_path_type_rotation) 
			{
				QuaternionTrack& track =
					result[i][nodeId].GetRotationTrack();
				GLTFHelpers::TrackFromChannel<quat, 4>
					(track, channel);
			}
		}
		result[i].RecalculateDuration();
	} // End num clips loop
	return result;
}

//保证每个关节都有一个默认姿势
Pose LoadBindPose(cgltf_data* data)
{
	//先获取rest下的局部姿势，即源矩阵数据转换的变换数据
	Pose restPose = LoadRestPose(data);
	unsigned int numBones = restPose.Size();
	std::vector<transform> worldBindPose(numBones);
	//计算全局姿势
	for (unsigned int i = 0; i < numBones; ++i) 
	{
		worldBindPose[i] = restPose.GetGlobalTransform(i);
	}
	
	unsigned int numSkins = data->skins_count;
	for (unsigned int i = 0; i < numSkins; ++i) 
	{
		cgltf_skin* skin = &(data->skins[i]);
		std::vector<float> invBindAccessor;
		GLTFHelpers::GetScalarValues(invBindAccessor, 16, *skin->inverse_bind_matrices);
		unsigned int numJoints = skin->joints_count;
		for (int j = 0; j < numJoints; ++j) 
		{
			// 读取每关节绑定姿势的逆（全局空间）
			float* matrix = &(invBindAccessor[j * 16]);
			mat4 invBindMatrix = mat4(matrix);
			// 将之转为SQT格式后求逆
			transform invbindTransform = mat4ToTransform(invBindMatrix);
			transform bindTransform = inverse(invbindTransform);
			// 存储此变换数据
			cgltf_node* jointNode = skin->joints[j];
			int jointIndex = GLTFHelpers::GetNodeIndex(
				jointNode, data->nodes, numBones);
			worldBindPose[jointIndex] = bindTransform;
		} // end for each joint
	} // end for each skin
	//将每个关节在bindPose下的全局姿势转换为局部姿势
	Pose bindPose = restPose;
	for (unsigned int i = 0; i < numBones; ++i) 
	{
		transform current = worldBindPose[i];
		int p = bindPose.GetParent(i);
		if (p >= 0) 
		{
			transform parent = worldBindPose[p];
			//与蒙皮矩阵的第一步类似
			current = combine(inverse(parent), current);
		}
		bindPose.SetLocalTransform(i, current);
	}
	return bindPose;
}

Skeleton LoadSkeleton(cgltf_data* data) 
{
	return Skeleton(
		LoadRestPose(data),
		LoadBindPose(data),
		LoadJointNames(data)
	);
}

std::vector<Mesh> LoadMeshes(cgltf_data* data)
{
	std::vector<Mesh> result;
	cgltf_node* nodes = data->nodes;
	unsigned int nodeCount = data->nodes_count;
	for (unsigned int i = 0; i < nodeCount; ++i) 
	{
		cgltf_node* node = &nodes[i];
		if (node->mesh == 0 || node->skin == 0) 
		{
			continue;
		}
		int numPrims = node->mesh->primitives_count;
		//遍历图元
		for (int j = 0; j < numPrims; ++j) 
		{
			result.push_back(Mesh());
			Mesh& mesh = result[result.size() - 1];
			cgltf_primitive* primitive =
				&node->mesh->primitives[j];
			//遍历图元属性
			unsigned int ac = primitive->attributes_count;
			for (unsigned int k = 0; k < ac; ++k) 
			{
				cgltf_attribute* attribute =
					&primitive->attributes[k];
				GLTFHelpers::MeshFromAttribute(mesh,
					*attribute, node->skin,
					nodes, nodeCount);
			}
			if (primitive->indices != 0) 
			{
				int ic = primitive->indices->count;
				std::vector<unsigned int>& indices =
					mesh.GetIndices();
				indices.resize(ic);
				for (unsigned int k = 0; k < ic; ++k) 
				{
					indices[k] = cgltf_accessor_read_index(
						primitive->indices, k);
				}
			}
			mesh.UpdateOpenGLBuffers();
		}
	}
	return result;
}

std::vector<Mesh> LoadStaticMeshes(cgltf_data* data) 
{
	std::vector<Mesh> result;
	cgltf_node* nodes = data->nodes;
	unsigned int nodeCount = (unsigned int)data->nodes_count;

	for (unsigned int i = 0; i < nodeCount; ++i) 
	{
		cgltf_node* node = &nodes[i];
		if (node->mesh == 0) 
		{
			continue;
		}

		unsigned int numPrims = (unsigned int)node->mesh->primitives_count;
		for (unsigned int j = 0; j < numPrims; ++j) 
		{
			result.push_back(Mesh());
			Mesh& mesh = result[result.size() - 1];

			cgltf_primitive* primitive = &node->mesh->primitives[j];

			unsigned int numAttributes = (unsigned int)primitive->attributes_count;
			for (unsigned int k = 0; k < numAttributes; ++k) 
			{
				cgltf_attribute* attribute = &primitive->attributes[k];
				GLTFHelpers::StaticMeshFromAttribute(mesh, *attribute);
			}

			if (primitive->indices != 0) 
			{
				unsigned int indexCount = (unsigned int)primitive->indices->count;
				std::vector<unsigned int>& indices = mesh.GetIndices();
				indices.resize(indexCount);

				for (unsigned int k = 0; k < indexCount; ++k) 
				{
					indices[k] = (unsigned int)cgltf_accessor_read_index(primitive->indices, k);
				}
			}
			mesh.UpdateOpenGLBuffers();
		}
	}
	return result;
}