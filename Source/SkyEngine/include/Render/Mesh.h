#pragma once
#include "EngineBase.h"
#include "Render/SceneView.h"

#include <vector>
//0-position;1-normal;2-color;3-uv;4-uv2;
#define ATTRIBUTE_POSITION            1   // 2^0, bit 0
#define ATTRIBUTE_NORMAL				2  // 2^1, bit 1
#define ATTRIBUTE_COLOR				4  // 2^2, bit 2
#define ATTRIBUTE_UV_COORD1          8  // 2^3, bit 3
#define ATTRIBUTE_UV_COORD2         16  // 2^4, bit 4


struct MeshFileHeader
{
	/* Unique 64-bit value to check integrity of the file */
	uint32 magicValue;

	uint32 vertexDataLayoutMask;

	/* The offset to combined mesh data (this is the base from which the offsets in individual meshes start) */
	uint32 dataBlockStartOffset;

	/* How much space index data takes */
	uint32 indexDataSize;

	/* How much space vertex data takes */
	uint32 vertexDataSize;
};


class Mesh
{
public:
	enum VertexAttribute
	{
		POSITION_ATTRIBUTE,
		NORMAL_ATTRIBUTE,
		COLOR_ATTRIBUTE,
		UV_COORD_ATTRIBUTE,
		UV2_COORD_ATTRIBUTE,
		NUM_VERTEX_ATTRIBUTES
	};

	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Color> colors;
	std::vector<Vector2> uvs;
	std::vector<Vector2> uv2s;
	std::vector<uint32> triangles;
	string name;
	BoundingBox bounds;
	
	//0-position;1-normal;2-color;3-uv;4-uv2;
	uint32 vertexDataLayoutMask;

	uint32 GetVertexDataSize() const;
	uint32 vertexCount;
	uint32 indiceCount;

protected:
	uint32 vaoID;									//Vertex Array Object
	uint32 vboIDs[Mesh::NUM_VERTEX_ATTRIBUTES];		//Vertex Buffer Object (One per vertex attribute)
	uint32 iboID;

public:
	Mesh(std::vector<Vector3> positions, std::vector<Vector3> normals, std::vector<uint32> triangles);
	Mesh(string binaryMeshPath);
	Mesh();
	virtual ~Mesh() {}

	inline std::vector<Vector3>& GetPositions() { return positions; }
	inline std::vector<Vector3>& GetNormals() { return normals; }
	inline std::vector<Vector2>& GetUvCoords() { return uvs; }
	inline std::vector<Color>& GetColors() { return colors; }
	inline std::vector<uint32>& GetIndices() { return triangles; }

	inline uint32 GetVAO() const { return vaoID; }
	inline uint32 GetVBO(uint32 index) const { return vboIDs[index]; }
	inline const uint32* GetVBOs() const { return vboIDs; }
	inline uint32 GetIBO() const { return iboID; }


	void BindBuffer();

	void CreateBufferData();


	inline void FreeData()
	{
		positions.clear();
		normals.clear();
		uvs.clear();
		triangles.clear();
		normals.clear();
	}
};


class CubeMesh : public Mesh
{
public:
	CubeMesh();
};


class QuadMesh : public Mesh
{
public:
	QuadMesh();
};


class SphereMesh : public Mesh
{
public:
	SphereMesh(int row, int col);
};
