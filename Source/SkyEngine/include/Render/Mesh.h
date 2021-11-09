#pragma once
#include "Base.h"
#include "Texture.h"

class TextureVariable;

//All primitives share a very simple interface and only vary by their setup implementation
struct Primitive {
	Primitive(unsigned int numVertex) : numVertices(numVertex) {};
	void Draw(const unsigned int readTex1 = 0, const unsigned int readTex2 = 0, const unsigned int readTex3 = 0);
	virtual void Setup() = 0;
	unsigned int VAO, VBO;
	const unsigned int numVertices;
};

//Mostly used for screen space or render to texture stuff
struct Quad : public Primitive {
	Quad() : Primitive(6) {};
	void Draw(const unsigned int readTex1 = 0, const unsigned int readTex2 = 0, const unsigned int readTex3 = 0);
	void Setup();
};

//Used in cubemap rendering
struct Cube : public Primitive {
	Cube() : Primitive(36) {};
	void Setup();
};


class Mesh
{
public:
	enum VertexAttribute
	{
		POSITION_ATTRIBUTE,
		UV_COORD_ATTRIBUTE,
		NORMAL_ATTRIBUTE,
		COLOR_ATTRIBUTE,
		NUM_VERTEX_ATTRIBUTES
	};

	vector<Vector3> positions;
	vector<Vector3> normals;
	vector<Color> colors;
	vector<Vector2> uvs;
	vector<uint32> triangles;
	vector<TextureVariable*> textures;
	int DefaultMaterialIndex;
	string Name;

protected:

	
	uint32 vaoID;									//Vertex Array Object
	uint32 vboIDs[Mesh::NUM_VERTEX_ATTRIBUTES];		//Vertex Buffer Object (One per vertex attribute)
	uint32 iboID;

public:
	Mesh(vector<Vector3> positions, vector<Vector3> normals, vector<Vector2> uv, vector<uint32> triangles, vector<TextureVariable*> textures);
	Mesh();
	virtual ~Mesh() {}

	inline vector<Vector3>& GetPositions() { return positions; }
	inline vector<Vector3>& GetNormals() { return normals; }
	inline vector<Vector2>& GetUvCoords() { return uvs; }
	inline vector<Color>& GetColors() { return colors; }
	inline vector<uint32>& GetIndices() { return triangles; }

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
