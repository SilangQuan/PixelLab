#include <iostream>
#include <fstream>
#include "nlohmann/fifo_map.hpp"
#include "../../LibAppFramework/include/application.h"
using namespace std;
using namespace nlohmann;

// A workaround to give to use fifo_map as map, we are just ignoring the 'less' compare
template<class K, class V, class dummy_compare, class A>
using rworkaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using rjson = basic_json<rworkaround_fifo_map>;

void SaveMaterialDescription(const char* fileName, const MaterialDescription& Description)
{
	qDebug() << "Save MaterialDescription: " << fileName;
	rjson matJson;
	matJson["name"] = Description.name;
	matJson["shader"] = Description.shader;
	matJson["albedoMap"] = Description.albedoMap;
	matJson["emissiveMap"] = Description.emissiveMap;
	matJson["metallicRoughnessMap"] = Description.metallicRoughnessMap;
	matJson["normalMap"] = Description.normalMap;
	matJson["opacityMap"] = Description.opacityMap;
	matJson["ambientOcclusionMap"] = Description.ambientOcclusionMap;

	matJson["emissiveColor"] = Description.emissiveColor.ToString();
	matJson["albedoColor"] = Description.albedoColor.ToString();
	matJson["roughness"] = Description.roughness.ToString();
	
	matJson["flags"] = std::to_string(Description.flags);

	//Back
	matJson["cullMode"] = "2";
	//Solid
	matJson["fillMode"] = "2";
	//LEquel
	matJson["zTest"] = "4";
	//On
	matJson["zWrite"] = "0";

	string name;


	ofstream outfile;
	outfile.open(fileName);
	 
	outfile << matJson.dump(4) << std::endl;

	outfile.close();
}

void SaveMeshData(const char* fileName, const Mesh& mesh)
{
	qDebug() <<"SaveMeshData: " << fileName;

	mesh.name.c_str();
	FILE* f = fopen(fileName, "wb");

	const MeshFileHeader header = {
		0x12345678,
		mesh.vertexDataLayoutMask,
		(uint32)(sizeof(MeshFileHeader)) + (uint32)(sizeof(BoundingBox)),
		(uint32)(mesh.triangles.size() * sizeof(uint32)),
		(uint32)(mesh.vertexCount * mesh.GetVertexDataSize() * sizeof(float))};

	std::vector<float> vertexData;

	for (size_t i = 0; i != mesh.vertexCount; i++)
	{
		if (mesh.vertexDataLayoutMask & ATTRIBUTE_POSITION)
		{
			vertexData.push_back(mesh.positions[i].x);
			vertexData.push_back(mesh.positions[i].y);
			vertexData.push_back(mesh.positions[i].z);
		}

		if (mesh.vertexDataLayoutMask & ATTRIBUTE_NORMAL)
		{
			vertexData.push_back(mesh.normals[i].x);
			vertexData.push_back(mesh.normals[i].y);
			vertexData.push_back(mesh.normals[i].z);
		}

		if (mesh.vertexDataLayoutMask & ATTRIBUTE_COLOR)
		{
			vertexData.push_back(mesh.colors[i].r);
			vertexData.push_back(mesh.colors[i].g);
			vertexData.push_back(mesh.colors[i].b);
			vertexData.push_back(mesh.colors[i].a);
		}

		if (mesh.vertexDataLayoutMask & ATTRIBUTE_UV_COORD1)
		{
			vertexData.push_back(mesh.uvs[i].x);
			vertexData.push_back(mesh.uvs[i].y);
		}

		if (mesh.vertexDataLayoutMask & ATTRIBUTE_UV_COORD2)
		{
			vertexData.push_back(mesh.uv2s[i].x);
			vertexData.push_back(mesh.uv2s[i].y);
		}
	}

	fwrite(&header, sizeof(header), 1, f);
	fwrite(&(mesh.bounds), sizeof(BoundingBox), 1, f);
	fwrite(mesh.triangles.data(),  header.indexDataSize, 1, f);
	fwrite(vertexData.data(), header.vertexDataSize, 1, f);

	fclose(f);
}

MeshFileHeader LoadMeshData(const char* fileName, Mesh& out)
{
	MeshFileHeader header;

	FILE* f = fopen(fileName, "rb");

	assert(f); 

	if (!f)
	{
		qDebug() << "File not found:" << fileName;
		exit(EXIT_FAILURE);
	}

	if (fread(&header, 1, sizeof(header), f) != sizeof(header))
	{
		printf("Unable to read mesh file header\n");
		exit(EXIT_FAILURE);
	}

	if (fread(&(out.bounds), sizeof(BoundingBox), 1, f) != sizeof(BoundingBox))
	{
		printf("Could not read bounding boxes\n");
		exit(255);
	}

	out.triangles.resize(header.indexDataSize / sizeof(uint32_t));

	std::vector<float> vertexData;

	vertexData.resize(header.vertexDataSize / sizeof(float));

	if ((fread(out.triangles.data(), 1, header.indexDataSize, f) != header.indexDataSize) ||
		(fread(vertexData.data(), 1, header.vertexDataSize, f) != header.vertexDataSize))
	{
		printf("Unable to read index/vertex data\n");
		exit(255);
	}

	fclose(f);

	return header;
}

int main(int argc, char *argv[])
{
	//string source = "../../BuiltinAssets/model/bunny.obj";
	//string source = "../../BuiltinAssets/model/bio.obj";
	//string source = "../../BuiltinAssets/model/Bistro_Interior/interior.obj";

	//string source = "../../BuiltinAssets/model/alucy.obj";
	//string source = "../../BuiltinAssets/model/sphere.obj";
	//string source = "../../BuiltinAssets/model/Cube.obj";
	string source = "../../BuiltinAssets/model/Bistro/Bistro.gltf";
	
	//string source = "../../BuiltinAssets/model/venusm2.obj";
	//string source = "../../BuiltinAssets/model/SphereMax.FBX";
	//string source = "../../BuiltinAssets/model/box.obj";
	//string source = "../../BuiltinAssets/model/Bistro_Motor2/bistro_motor2.gltf";
	//string source = "../../BuiltinAssets/model/Sponza/sponza.gltf";
	//string source = "../../BuiltinAssets/model/DamagedHelmet/DamagedHelmet.gltf";
	//string source = "../../BuiltinAssets/model/teapot.obj";
	
	//string source = "../../BuiltinAssets/model/Bistro_RoadLight/Bistro_RoadLight.gltf";
	//string targetFolder = "../../Library/Mesh/Bistro_RoadLight/";
	string targetFolder = "../../Library/Mesh/";
	
	Model model(source.c_str());
	vector<Mesh>* meshes = model.GetMeshes();
	for (int i = 0; i < meshes->size(); i++)
	{
		string target = targetFolder+meshes->at(i).name /*+ std::to_string(i)*/  +  string(".mesh");
		SaveMeshData(target.c_str(), meshes->at(i));
	}

	targetFolder = "../../Library/Material/";
	vector<MaterialDescription>* MaterialDescriptions = model.GetMaterialDescription();
	for (int i = 0; i < MaterialDescriptions->size(); i++)
	{
		string target = targetFolder + MaterialDescriptions->at(i).name /* + std::to_string(i)*/ + string(".material");
		MaterialDescriptions->at(i).shader = "pbr_opaque";
		SaveMaterialDescription(target.c_str(), MaterialDescriptions->at(i));
	}

	qDebug() << "Done";

	return 0;
}
