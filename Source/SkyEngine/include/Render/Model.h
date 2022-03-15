#pragma once
#include "../Base.h"
#include "../Core/ObjLoader.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/version.h"
#include "assimp/pbrmaterial.h"
#include "assimp/postprocess.h"
#include "Render/Material.h"




class ShaderProgram;
class Mesh;
class TextureVariable;
class Camera;
class RenderContext;

namespace tinygltf {
	class Scene;
	class Model;
}
struct TfNode
{
	std::vector<int> Childrens;

	int MeshIndex = -1;

	std::string Name;

	Transform LocalTrans;
};

struct TfMeshPrimitive
{
	int MeshIndex = -1;
	int MaterialIndex = -1;
	std::string Name;
	Mesh* MeshData;
};

struct TfMesh
{
	vector<TfMeshPrimitive> Primitives;
	std::string Name;
};


struct TfTexture
{
	Texture* TexData;
	std::string Name;
};

class Model
{
public:
	Model(const char* path, bool usingAssimp = true);
	~Model();
	void Draw(ShaderProgram* shader);
	void DrawMesh(Material* mat, Matrix4x4* model, Camera* cam, RenderContext* contex);
	void CreateBufferData();
	vector<Mesh>* GetMeshes();
	vector<MaterialDescription>* GetMaterialDescription() { return &mMaterialDescriptions; };
	vector<TfNode>* GetNodes() { return &mNodes; };
	vector<TfMesh>* GetTfMeshes() { return &mTfMeshes; };
	vector<TfTexture>* GetTfTextures() { return &mTfTextures; };
	vector<TfMaterial>* GetTfMats() { return &mTfMaterials; };
	EModelType ModelType = EModelType::OBJ;


private:
	vector<Mesh> mMeshes;
	vector<Material> mMaterials;
	vector<MaterialDescription> mMaterialDescriptions;

	vector<TfMaterial> mTfMaterials;
	vector<TfNode> mNodes;
	vector<TfMesh> mTfMeshes;
	vector<TfTexture> mTfTextures;

	string directory;

	void LoadModel(string path);
	Mesh  ProcessMesh(tinyobj::ObjMesh* mesh, vector<tinyobj::ObjMaterial>& materials);

	void LoadModelByAssimp(string path);

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void ProcessNodeByAssimp(aiNode* node, const aiScene* scene);

	Mesh ProcessMeshByAssimp(aiMesh* mesh, const aiScene* scene);

	void ProcessAIMaterials(const aiScene* scene);

	void LoadModelByGltf(const string& path, bool binary);

	void ProcessGltfNode(tinygltf::Scene* scene, tinygltf::Model* gltfModel);
	void ProcessGltfMesh(tinygltf::Model* gltfModel);
	void ProcessGltfTexture(tinygltf::Model* gltfModel);
	void ProcessGltfMaterial(string directory, tinygltf::Model* model, vector<TfMaterial>& mats);

	//void ProcessGltfNode(tinygltf::Model* model, tinygltf::Node* node);
	//void ProcessGltfMesh(tinygltf::Model* model, tinygltf::Node* node);
	//void ProcessGltfMaterial(tinygltf::Model* model);

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	vector<TextureVariable*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

	//TextureVariable* LoadTexture(string name);
};