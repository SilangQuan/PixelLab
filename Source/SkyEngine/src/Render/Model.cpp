#include "Render/model.h"
#include "Render/ShaderProgram.h"
#include "Render/Mesh.h"
#include "Core/ObjLoader.h"
#include "Render/Graphics.h"


#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include "Render/tiny_gltf.h"



static std::string GetFilePathExtension(const std::string& FileName) {
	if (FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(FileName.find_last_of(".") + 1);
	return "";
}

Model::Model(const char* path, bool usingAssimp)
{
	string input = path;
	int index = input.find_last_of("/");
	directory = input.substr(0, index + 1);
	
	std::string ext = GetFilePathExtension(input);

	bool ret = false;

	if (ext.compare("fbx") == 0)
	{
		ModelType = EModelType::FBX;
	}
	else if (ext.compare("obj") == 0)
	{
		ModelType = EModelType::OBJ;
	}


	if (ext.compare("glb") == 0) {
		LoadModelByGltf(input, true);
		ModelType = EModelType::GLTF;
		return;
	}
	else if (ext.compare("gltf") == 0) {
		ModelType = EModelType::GLTF;
		LoadModelByGltf(input, false);
		return;
	}

	if (usingAssimp)
	{
		LoadModelByAssimp(path);
	}
	else
	{
		LoadModel(path);
	}
}

Model::~Model()
{

}

void Model::CreateBufferData()
{
	if (ModelType == EModelType::GLTF)
	{
		for (std::vector<TfMesh>::iterator iter = mTfMeshes.begin(); iter != mTfMeshes.end(); ++iter)
		{
			for (int j = 0; j < iter->Primitives.size(); j++)
			{
				if(iter->Primitives[j].MeshData != NULL)
					iter->Primitives[j].MeshData->CreateBufferData();
			}
		}
	}else
	{
		for (std::vector<Mesh>::iterator iter = meshes.begin(); iter != meshes.end(); ++iter)
		{
			iter->CreateBufferData();
		}
	}
}

// Render the mesh
void Model::Draw(ShaderProgram* shader)
{
	
}

void Model::DrawMesh(Material* mat, Matrix4x4* model, Camera* cam, RenderContext* contex)
{
	for (std::vector<Mesh>::iterator iter = meshes.begin(); iter != meshes.end(); ++iter)
	{
		Graphics::DrawMesh(&(*iter), model,mat, cam, contex);
	}
}


vector<Mesh>* Model::GetMeshes()
{
	return &meshes;
}

void Model::LoadModel(string path)
{
	vector<tinyobj::ObjMesh> shapes;
	vector<tinyobj::ObjMaterial> materials;

	string err;
	bool ret = tinyobj::ObjLoader::LoadObj(shapes, materials, err, path.c_str(), directory.c_str());
	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << endl;
	}

	if (!ret) {
		qDebug() << "LoadModel" << path << " error";
		exit(1);
	}else
	{
		qDebug() << path << " Loaded!";
		qDebug() << "shapes.size()" << (int)shapes.size();
		qDebug() << "materials.size()" << (int)materials.size();
	}

	for (int i = 0; i < shapes.size(); i++)
	{
		qDebug() << "shapes: " << shapes[i].name;
		qDebug() << "shapes: " << (int)shapes[i].material_ids.size();
		meshes.push_back(ProcessMesh(&shapes[i], materials));
	}

	// Process materials
	for (size_t i = 0; i < materials.size(); i++)
	{
		qDebug() << "materials: " << materials[i].diffuse_texname.c_str();
	}
}


		//aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
/*
		vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;
}*/


Mesh Model::ProcessMesh(tinyobj::ObjMesh* mesh, vector<tinyobj::ObjMaterial>& materials)
{
	vector<Vector3> positions;
	vector<Vector3> normals;
	vector<Vector2> uvs;
	vector<GLuint> indices = mesh->indices;
	vector<TextureVariable*> textures;

	positions.reserve(mesh->positions.size() / 3);
	normals.reserve(mesh->positions.size() / 3);
	uvs.reserve(mesh->positions.size() / 3);
	//memcpy(positions.data(), mesh->positions.data(), sizeof(Vector3) * positions.size());

	bool hasUV = mesh->texcoords.size() > 0;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->positions.size()/3; i++)
	{
		positions.push_back(Vector3(mesh->positions[3 * i + 0], mesh->positions[3 * i + 1], mesh->positions[3 * i + 2]));

		normals.push_back(Vector3(mesh->normals[3 * i + 0], mesh->normals[3 * i + 1], mesh->normals[3 * i + 2]));

		Vector2 tmpTexcoord(0,1);
		// Texture Coordinates
		if (hasUV) // Does the mesh contain texture coordinates?
		{
			tmpTexcoord.x = mesh->texcoords[2 * i + 0];
			tmpTexcoord.y = mesh->texcoords[2 * i + 1];
		}

		uvs.push_back(tmpTexcoord);
	}

	qDebug() << mesh->name << (int)mesh->material_ids.size();

/*
	for (int i = 0; i < mesh->material_ids.size(); i++)
	{
		tinyobj::ObjMaterial mat = materials[mesh->material_ids[i]];
		qDebug() << mesh->name<< " " << mat.diffuse_texname;
	}
*/

	// Return a mesh object created from the extracted mesh data
	return Mesh(positions, normals, uvs, indices, textures);
}


Texture* LoadTexture(string dir, string name)
{
	// If texture hasn't been loaded already, load it
	string path = dir  + name.c_str();

	Texture* texture = ResourceManager::GetInstance()->TryGetResource<Texture>(path);

	return texture;
	//TextureVariable* textureVariable = new TextureVariable(texture, textureUnit, uniformName);
	//return textureVariable;
}

void Model::ProcessGltfMaterial(string directory, tinygltf::Model* model, vector<TfMaterial>& mats)
{
	for (size_t i = 0; i < model->materials.size(); i++)
	{
		const tinygltf::Material& material = model->materials[i];
		TfMaterial pbrMat;
		pbrMat.alphaCutoff = material.alphaCutoff;
		pbrMat.alphaMode = material.alphaMode == "OPAQUE" ? ALPHAMODE_OPAQUE : ALPHAMODE_BLEND;
		pbrMat.name = material.name;
		//auto pbrMetallicRoughnessIt = material.find("pbrMetallicRoughness");
	//	if (pbrMetallicRoughnessIt != material.end())

	
		//Need to support specular workflow
		pbrMat.workflow = METALLIC_ROUGHNESS;

		pbrMat.metallicFactor = material.pbrMetallicRoughness.metallicFactor;
		pbrMat.roughnessFactor = material.pbrMetallicRoughness.roughnessFactor;
		pbrMat.baseColorFactor = Color::MakeColor(material.pbrMetallicRoughness.baseColorFactor.data());
		pbrMat.emissiveFactor = Color::MakeColor(material.emissiveFactor.data());
		
		//Albedo
		if (model->images.size() > 0 && material.pbrMetallicRoughness.baseColorTexture.index != -1 && &model->images[0] + material.pbrMetallicRoughness.baseColorTexture.index != NULL)
			//pbrMat.baseColorTexture = LoadTexture(directory, "albedoMap", model->images[material.pbrMetallicRoughness.baseColorTexture.index].uri, 0);
			pbrMat.baseColorTexture = new TextureVariable(mTfTextures[material.pbrMetallicRoughness.baseColorTexture.index].TexData, 0, "albedoMap");
		else
			pbrMat.baseColorTexture = new TextureVariable(ResourceManager::GetInstance()->FindTexture("WhiteDummy"), 0, "albedoMap");
		
		//metallicRoughnessMap
		if (model->images.size() > 0 && material.pbrMetallicRoughness.metallicRoughnessTexture.index != -1 && &model->images[0] + material.pbrMetallicRoughness.metallicRoughnessTexture.index != NULL)
			//pbrMat.metallicRoughnessTexture = LoadTexture(directory, "metallicRoughnessMap", model->images[material.pbrMetallicRoughness.metallicRoughnessTexture.index].uri, 1);
			pbrMat.metallicRoughnessTexture = new TextureVariable(mTfTextures[material.pbrMetallicRoughness.metallicRoughnessTexture.index].TexData, 1, "metallicRoughnessMap");
		else
			pbrMat.metallicRoughnessTexture = new TextureVariable(ResourceManager::GetInstance()->FindTexture("WhiteDummy"), 1, "metallicRoughnessMap");

		//normalMap
		if (model->images.size() > 0 && material.normalTexture.index != -1 && &model->images[0] + material.normalTexture.index != NULL)
			pbrMat.normalTexture = new TextureVariable(mTfTextures[material.normalTexture.index].TexData, 2, "normalMap");
			//pbrMat.normalTexture = LoadTexture(directory, "normalMap", model->images[material.normalTexture.index].uri, 2);
		else
			pbrMat.normalTexture = new TextureVariable(ResourceManager::GetInstance()->FindTexture("DefaultNormal"), 2, "normalMap");


		//aoMap
		if (model->images.size() > 0 && material.occlusionTexture.index != -1 && &model->images[0] + material.occlusionTexture.index != NULL)
			pbrMat.occlusionTexture = new TextureVariable(mTfTextures[material.occlusionTexture.index].TexData, 3, "aoMap");
			//pbrMat.occlusionTexture = LoadTexture(directory, "aoMap", model->images[material.occlusionTexture.index].uri, 3);
		else
			pbrMat.occlusionTexture = new TextureVariable(ResourceManager::GetInstance()->FindTexture("WhiteDummy"), 3, "aoMap");

		//emissiveMap
		if (model->images.size() > 0 && material.emissiveTexture.index != -1 && &model->images[0] + material.emissiveTexture.index != NULL)
			pbrMat.emissiveTexture = new TextureVariable(mTfTextures[material.emissiveTexture.index].TexData, 4, "emissiveMap");
			//pbrMat.emissiveTexture = LoadTexture(directory, "emissiveMap", model->images[material.emissiveTexture.index].uri, 4);
		else
			pbrMat.emissiveTexture = new TextureVariable(ResourceManager::GetInstance()->FindTexture("BlackDummy"), 4, "emissiveMap");


			//pbrMat.metallicRoughnessTexture = LoadTexture(directory, "normalMap", model->images[material.pbrMetallicRoughness.metallicRoughnessTexture.index].uri, 1);
			//pbrMat.normalTexture = LoadTexture(directory, "metallicRoughnessMap", model->images[material.normalTexture.index].uri, 2);
			//pbrMat.occlusionTexture = LoadTexture(directory, "aoMap", model->images[material.occlusionTexture.index].uri, 3);
			//pbrMat.emissiveTexture = LoadTexture(directory, "emissiveMap", model->images[material.emissiveTexture.index].uri, 4);
		
		mats.push_back(pbrMat);
	}
	/*
	ShaderProgram* shader = ResourceManager::GetInstance()->FindShader("PbrOpaque");
	for (int i = 0; i < mats.size(); i++)
	{
		Material* tmpMat = new Material(shader);
		tmpMat->Name = "Mat" + i;
		tmpMat->SetCullMode(CM_Back);
		
		tmpMat->SetColor("baseColorFactor", mats[i].baseColorFactor);
		tmpMat->SetFloat("metallicFactor", mats[i].metallicFactor);
		tmpMat->SetFloat("roughnessFactor", mats[i].roughnessFactor);
		tmpMat->SetColor("emissiveFactor", mats[i].emissiveFactor);

		tmpMat->AddTextureVariable(mats[i].baseColorTexture);
		tmpMat->AddTextureVariable(mats[i].normalTexture);
		tmpMat->AddTextureVariable(mats[i].metallicRoughnessTexture);
		tmpMat->AddTextureVariable(mats[i].occlusionTexture);
		tmpMat->AddTextureVariable(mats[i].emissiveTexture);

		ResourceManager::GetInstance()->AddMaterial(tmpMat);
	}*/
}

void Model::ProcessGltfTexture(tinygltf::Model* model)
{
	int imageCount = model->images.size();
	mTfTextures.resize(imageCount);
	for (int i = 0; i < imageCount; i++)
	{
		mTfTextures[i].TexData = LoadTexture(directory, model->images[i].uri);
		mTfTextures[i].Name = model->images[i].name;
	}
}


void Model::ProcessGltfMesh(tinygltf::Model* model)
{
	int meshCount = model->meshes.size();
	mTfMeshes.resize(meshCount);

	for (int i = 0; i < meshCount; i++)
	{
		const tinygltf::Mesh mesh = model->meshes[i];
		mTfMeshes[i].Primitives.resize(mesh.primitives.size());
		mTfMeshes[i].Name = mesh.name;

		vector<TextureVariable*> textures;

		for(int j = 0; j< mesh.primitives.size(); j++)
		{
			tinygltf::Primitive primitive = mesh.primitives[j];
			vector<Vector3> positions;
			vector<Vector3> normals;
			vector<Vector2> uvs;
			//vector<Vertex> vertices;
			vector<GLuint> indices;

			if (primitive.indices < 0) {
				continue;
			}
			// Vertices
			{
				const float* bufferPos = nullptr;
				const float* bufferNormals = nullptr;
				const float* bufferTexCoords = nullptr;

				// Position attribute is required
				assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

				const tinygltf::Accessor& posAccessor = model->accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& posView = model->bufferViews[posAccessor.bufferView];
				bufferPos = reinterpret_cast<const float*>(&(model->buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
					const tinygltf::Accessor& normAccessor = model->accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& normView = model->bufferViews[normAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float*>(&(model->buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
				}

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& uvAccessor = model->accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& uvView = model->bufferViews[uvAccessor.bufferView];
					bufferTexCoords = reinterpret_cast<const float*>(&(model->buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				}

				for (size_t v = 0; v < posAccessor.count; v++)
				{
					positions.push_back(Vector3(bufferPos[v * 3], bufferPos[v * 3 + 1], bufferPos[v * 3 + 2]));
					if (bufferNormals != NULL)
					{
						normals.push_back(Vector3(bufferNormals[v * 3], bufferNormals[v * 3 + 1], bufferNormals[v * 3 + 2]));
					}
					else
					{
						normals.push_back(Vector3::zero);
					}

					if (bufferTexCoords != NULL)
					{
						uvs.push_back(Vector2(bufferTexCoords[v * 2], bufferTexCoords[v * 2 + 1]));
					}
					else
					{
						uvs.push_back(Vector2(0, 0));
					}
				}
			}

			uint32_t vertexStart = 0;

			// Indices
			{
				const tinygltf::Accessor& accessor = model->accessors[primitive.indices];
				const tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = model->buffers[bufferView.buffer];

				indices.reserve(static_cast<uint32_t>(accessor.count));

				// each morph has own gl_VertexIndex start at 0 so index is at zero_
				switch (accessor.componentType) {
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
					uint32_t* buf = new uint32_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indices.push_back(buf[index] + vertexStart);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
					uint16_t* buf = new uint16_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indices.push_back(buf[index] + vertexStart);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
					uint8_t* buf = new uint8_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indices.push_back(buf[index] + vertexStart);
					}
					break;
				}
				default:
					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
					return;
				}
			}
			TfMeshPrimitive meshPrimitive;

			meshPrimitive.MeshData = new Mesh(positions, normals, uvs, indices, textures);
			meshPrimitive.MaterialIndex = primitive.material;
			//tempMesh.Name = mesh.name;
			mTfMeshes[i].Primitives[j] = meshPrimitive;
		}
	}
}

void Model::ProcessGltfNode(tinygltf::Scene* scene, tinygltf::Model* gltfModel)
{
	mNodes.resize(gltfModel->nodes.size());

	for (int i = 0; i < gltfModel->nodes.size(); i++)
	{
		TfNode* tfnode = &mNodes[i];
		tinygltf::Node node = gltfModel->nodes[i];
		if (node.children.size()  > 0)
		{
			for (int c = 0; c < node.children.size(); c++)
			{
				int nodeID = node.children[c];
				tfnode->Childrens.push_back(nodeID);
			}
		}

		tfnode->MeshIndex = node.mesh;
		tfnode->Name = node.name;

		if(node.translation.size()>0)
			tfnode->LocalTrans.position = Vector3(node.translation[0], node.translation[1], node.translation[2]);
		if (node.scale.size() > 0)
			tfnode->LocalTrans.scale = Vector3(node.scale[0], node.scale[1], node.scale[2]);
		//tfnode->LocalTrans.rotation = Quaternion::identity;
		if (node.rotation.size() > 0)
		{
			tfnode->LocalTrans.rotation = Quaternion(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
			//tfnode->LocalTrans.rotation = math::Matrix4(math::Quat(GetVector(node["rotation"].get<json::array_t>())), math::Vector3(0.f, 0.f, 0.f));
		}
		else
		{
			tfnode->LocalTrans.rotation = Quaternion::identity;
		}
	}
}

void Model::LoadModelByGltf(const string& path, bool binary)
{
	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF gltfContext;
	std::string error;
	std::string warning;
	bool fileLoaded = false;
	if (!binary)
	{
		fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, path.c_str());
	}
	else
	{
		fileLoaded = gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, path.c_str());
	}

	if (fileLoaded) {

		ProcessGltfTexture(&gltfModel);
		ProcessGltfMaterial(directory, &gltfModel, mTfMaterials);

		tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene];
		ProcessGltfNode(&scene, &gltfModel);
		ProcessGltfMesh(&gltfModel);

		/*
		for (size_t i = 0; i < scene->nodes.size(); i++) {
			tinygltf::Node node = gltfModel.nodes[scene->nodes[i]];
			ProcessGltfNode(&gltfModel, &node, meshes);
		}*/
	}
	else {
		// TODO: throw
		std::cerr << "Could not load gltf file: " << error << std::endl;
		exit(-1);
	}

}


void Model::LoadModelByAssimp(string path)
{
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FlipWindingOrder);
	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// Retrieve the directory path of the filepath
	this->directory = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	this->ProcessNodeByAssimp(scene->mRootNode, scene);
}

Mesh Model::ProcessMeshByAssimp(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	vector<Vector3> positions;
	vector<Vector3> normals;
	vector<Vector2> uvs;
	//vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<TextureVariable*> textures;

	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		//Vertex vertex;
		Vector3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder Vector3 first.
						  // Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		positions.push_back(vector);
		// Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		normals.push_back(vector);

		Vector2 tmpVec2(0, 0);
		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			tmpVec2.x = mesh->mTextureCoords[0][i].x;
			tmpVec2.y = mesh->mTextureCoords[0][i].y;
		}
		uvs.push_back(tmpVec2);

	}
	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		/*
		//Finding current texture directory
		aiString texturePath;
		aiTextureType type;
		std::string fullTexturePath;

		for (int tex = aiTextureType_NONE; tex <= aiTextureType_UNKNOWN; tex++)
		{
			aiTextureType type = static_cast<aiTextureType>(tex); //making the int value into the enum value
			fullTexturePath = directory;

			//If there are any textures of the given type in the material
			if (material->GetTextureCount(type) > 0) {
				//We only care about the first texture assigned we don't expect multiple to be assigned
				material->GetTexture(type, 0, &texturePath);
			}
		}

		for (int i = 0; i < material->mNumProperties; i++)
		{
			qDebug() <<material->mProperties[i]->mKey.C_Str();
			qDebug()<< material->mProperties[i]->mData;
		}

		*/


		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
		vector<TextureVariable*> diffuseMaps = this->LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		vector<TextureVariable*> specularMaps = this->LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	// Return a mesh object created from the extracted mesh data
	return Mesh(positions, normals, uvs, indices, textures);
}

void Model::ProcessNodeByAssimp(aiNode* node, const aiScene* scene)
{
	// Process each mesh located at the current node
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->ProcessMeshByAssimp(mesh, scene));
	}
	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNodeByAssimp(node->mChildren[i], scene);
	}
}

vector<TextureVariable*> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<TextureVariable*> tmpTextures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// If texture hasn't been loaded already, load it
		string path = this->directory + "/" + str.C_Str();
			
		Texture* texture = ResourceManager::GetInstance()->TryGetResource<Texture>(path);

		stringstream ss;
		ss << "material.";
		ss << typeName;
		ss << i;
		TextureVariable* textureVariable = new TextureVariable(texture, -1, ss.str());

		tmpTextures.push_back(textureVariable);
	}
	return tmpTextures;
}
