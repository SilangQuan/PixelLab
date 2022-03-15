#include "Render/Mesh.h"
#include "Render/ShaderProgram.h"

Mesh::Mesh() : positions(), normals(), uvs(), triangles(), vaoID(-1), iboID(-1)
{
	for (uint32 i = 0; i < (uint32)Mesh::NUM_VERTEX_ATTRIBUTES; i++)
	{
		vboIDs[i] = -1;
	}
}
Mesh::Mesh(vector<Vector3> inPositions, vector<Vector3> inNormals, vector<uint32> inTriangles): positions(inPositions), normals(inNormals), triangles(inTriangles), vaoID(-1), iboID(-1)
{
	//Bouding box generation.
	Vector3 vmin(Mathf::Infinity);
	Vector3 vmax(Mathf::NegativeInfinity);
	for (auto i = 0; i != inPositions.size(); i++)
	{
		vmin = Vector3::Min(vmin, inPositions[i]);
		vmax = Vector3::Max(vmax, inPositions[i]);
	}
	bounds = BoundingBox(vmin, vmax);
	 
	indiceCount = inTriangles.size();
	vertexCount = inPositions.size();
}

Mesh::Mesh(string binaryMeshPath)
{
	MeshFileHeader header;

	FILE* f = fopen(binaryMeshPath.c_str(), "rb");

	assert(f); 

	if (!f)
	{
		qDebug() << "Mesh File not found - " << binaryMeshPath;
	}

	if (fread(&header, 1, sizeof(header), f) != sizeof(header))
	{
		printf("Unable to read mesh file header\n");
	}

	vertexDataLayoutMask = header.vertexDataLayoutMask;
	indiceCount = header.indexDataSize / sizeof(uint32);

	vertexCount = (header.vertexDataSize / sizeof(float)) / GetVertexDataSize();

	std::vector<float> vertexData;
	std::vector<uint32> indexData;

	if (fread(&bounds, sizeof(BoundingBox), 1, f) != 1)
	{
		printf("Could not read bounding boxes\n");
	}

	indexData.resize(header.indexDataSize / sizeof(uint32));
	vertexData.resize(header.vertexDataSize / sizeof(float));

	//fwrite(mesh.triangles.data(), header.indexDataSize, 1, f);
	//fwrite(vertexData.data(), header.vertexDataSize, 1, f);

	if ((fread(indexData.data(), header.indexDataSize, 1, f) !=1 ) ||
		(fread(vertexData.data(),  header.vertexDataSize, 1, f) != 1))
	{
		printf("Unable to read index/vertex data\n");
	}
	


	//Vertex data buffer
	glGenVertexArrays(1, &vaoID);

	//Only need one buffer now!
	glGenBuffers(1, &vboIDs[0]);
	//Index buffer
	glGenBuffers(1, &iboID);

	


	glBindVertexArray(vaoID);


	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
	//translate data to buffer
	glBufferData(GL_ARRAY_BUFFER, header.vertexDataSize, &(vertexData[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, header.indexDataSize, &(indexData[0]), GL_STATIC_DRAW);


	//Set layout
	glEnableVertexAttribArray(Mesh::POSITION_ATTRIBUTE);
	uint32 strideSize = GetVertexDataSize() * sizeof(GLfloat);

	//Param5 -stride size,every vertex data size
	//Param6 - data start pos pointer in stride
	glVertexAttribPointer(Mesh::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, strideSize, (GLvoid*)0);
	
	uint32 startPoint = 3;
	if (vertexDataLayoutMask & ATTRIBUTE_NORMAL)
	{
		glEnableVertexAttribArray(Mesh::NORMAL_ATTRIBUTE);
		glVertexAttribPointer(Mesh::NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, strideSize, (GLvoid*)(startPoint * sizeof(GLfloat)));
		startPoint += 3;
	}
	if (vertexDataLayoutMask & ATTRIBUTE_COLOR)
	{
		glEnableVertexAttribArray(Mesh::COLOR_ATTRIBUTE);
		glVertexAttribPointer(Mesh::COLOR_ATTRIBUTE, 4, GL_FLOAT, GL_FALSE, strideSize, (GLvoid*)(startPoint * sizeof(GLfloat)));
		startPoint += 4;
	}

	//Enable uvCoord attribute
	if (vertexDataLayoutMask & ATTRIBUTE_UV_COORD1)
	{
		glVertexAttribPointer(Mesh::UV_COORD_ATTRIBUTE,2, GL_FLOAT,GL_FALSE, strideSize, (GLvoid*)(startPoint * sizeof(GLfloat)));
		glEnableVertexAttribArray(Mesh::UV_COORD_ATTRIBUTE);
		startPoint += 2;
	}

	//Enable uv2Coord attribute
	if (vertexDataLayoutMask & ATTRIBUTE_UV_COORD2)
	{
		glVertexAttribPointer(Mesh::UV_COORD_ATTRIBUTE,2, GL_FLOAT,GL_FALSE,strideSize, (GLvoid*)(startPoint * sizeof(GLfloat)));
		glEnableVertexAttribArray(Mesh::UV2_COORD_ATTRIBUTE);
	}
	glBindVertexArray(0);

	



		/*
	GLfloat planeVertices[] = {
		// Positions          // Normals         // Texture Coords
		8.0f, -0.5f,  8.0f,  0.0f, 1.0f, 0.0f,  5.0f, 0.0f,
		-8.0f, -0.5f,  8.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		-8.0f, -0.5f, -8.0f,  0.0f, 1.0f, 0.0f,  0.0f, 5.0f,

		8.0f, -0.5f,  8.0f,  0.0f, 1.0f, 0.0f,  5.0f, 0.0f,
		-8.0f, -0.5f, -8.0f,  0.0f, 1.0f, 0.0f,  0.0f, 5.0f,
		8.0f, -0.5f, -8.0f,  0.0f, 1.0f, 0.0f,  5.0f, 5.0f
	};
	// Setup plane VAO
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboIDs[0]);
	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);

	*/
 	fclose(f);
}


uint32 Mesh::GetVertexDataSize() const
{
	int size = 0;
	if (vertexDataLayoutMask & ATTRIBUTE_POSITION)
	{
		size += 3;
	}

	if (vertexDataLayoutMask & ATTRIBUTE_NORMAL)
	{
		size += 3;
	}

	if (vertexDataLayoutMask & ATTRIBUTE_COLOR)
	{
		size += 4;
	}

	if (vertexDataLayoutMask & ATTRIBUTE_UV_COORD1)
	{
		size += 2;
	}

	if (vertexDataLayoutMask & ATTRIBUTE_UV_COORD2)
	{
		size += 2;
	}
	return size;
}

void Mesh::BindBuffer()
{
	//Bind Vertex Array
	glBindVertexArray(vaoID);

	//glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[Mesh::POSITION_ATTRIBUTE]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//glEnableVertexAttribArray(1));
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[Mesh::UV_COORD_ATTRIBUTE]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//glEnableVertexAttribArray(2));
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[Mesh::NORMAL_ATTRIBUTE]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


	if (this->GetIndices().size() > 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	}

}

void Mesh::CreateBufferData()
{
	//Generate gl buffers
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(Mesh::NUM_VERTEX_ATTRIBUTES, &vboIDs[0]);
	glGenBuffers(1, &iboID);

	//Bind Vertex Array
	glBindVertexArray(vaoID);

	//Copy position data into POSITION buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[Mesh::POSITION_ATTRIBUTE]);
	glBufferData(
		GL_ARRAY_BUFFER,
		this->GetPositions().size() * sizeof(this->GetPositions()[0]),
		&(this->GetPositions()[0]),
		GL_STATIC_DRAW);


	glVertexAttribPointer(
		Mesh::POSITION_ATTRIBUTE,
		3, GL_FLOAT,
		GL_FALSE,
		0, 0);
	//Enable position attribute
	glEnableVertexAttribArray(Mesh::POSITION_ATTRIBUTE);

	//Copy uvCoord data into UV_COORD buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[Mesh::UV_COORD_ATTRIBUTE]);
	glBufferData(
		GL_ARRAY_BUFFER,
		this->GetUvCoords().size() * sizeof(this->GetUvCoords()[0]),
		&(this->GetUvCoords()[0]),
		GL_STATIC_DRAW);

	glVertexAttribPointer(
		Mesh::UV_COORD_ATTRIBUTE,
		2, GL_FLOAT,
		GL_FALSE,
		0, 0);
	//Enable uvCoord attribute
	glEnableVertexAttribArray(Mesh::UV_COORD_ATTRIBUTE);

	//Copy normal data into NORMAL buffer
	if (normals.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[Mesh::NORMAL_ATTRIBUTE]);
		glBufferData(
			GL_ARRAY_BUFFER,
			this->GetNormals().size() * sizeof(this->GetNormals()[0]),
			&(this->GetNormals()[0]),
			GL_STATIC_DRAW);

		glVertexAttribPointer(
			Mesh::NORMAL_ATTRIBUTE,
			3, GL_FLOAT,
			GL_FALSE,
			0, 0);
		//Enable normal attribute
		glEnableVertexAttribArray(Mesh::NORMAL_ATTRIBUTE);
	}

	if(this->GetIndices().size() > 0)
	{
		//Copy index data to Element Array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			this->GetIndices().size() * sizeof(this->GetIndices()[0]),
			&(this->GetIndices()[0]),
			GL_STATIC_DRAW);
	}
	
	//Copy color data into COLOR_COORD buffer
	if (this->GetColors().size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vboIDs[Mesh::COLOR_ATTRIBUTE]);
		glBufferData(
			GL_ARRAY_BUFFER,
			this->GetColors().size() * sizeof(this->GetColors()[0]),
			&(this->GetColors()[0]),
			GL_STATIC_DRAW);

		glVertexAttribPointer(
			Mesh::COLOR_ATTRIBUTE,
			4, GL_FLOAT,
			GL_FALSE,
			0, 0);
		//Enable color attribute
		glEnableVertexAttribArray(Mesh::COLOR_ATTRIBUTE);
	}


	//Unbind Vertex Array
	glBindVertexArray(0);
}

CubeMesh::CubeMesh()
{
	GLfloat verticesData0[] = {
		0.5, -0.5, 0.5,0.0, 0.0, 1.0,0.0, 0.0,
		-0.5, -0.5, 0.5,0.0, 0.0, 1.0,1.0, 0.0,
		0.5, 0.5, 0.5,0.0, 0.0, 1.0,0.0, 1.0,
		-0.5, 0.5, 0.5,0.0, 0.0, 1.0,1.0, 1.0,
		0.5, 0.5, -0.5,0.0, 1.0, 0.0,0.0, 1.0,
		-0.5, 0.5, -0.5,0.0, 1.0, 0.0,1.0, 1.0,
		0.5, -0.5, -0.5,0.0, 0.0, -1.0,0.0, 1.0,
		-0.5, -0.5, -0.5,0.0, 0.0, -1.0,1.0, 1.0,
		0.5, 0.5, 0.5,0.0, 1.0, 0.0,0.0, 0.0,
		-0.5, 0.5, 0.5,0.0, 1.0, 0.0,1.0, 0.0,
		0.5, 0.5, -0.5,0.0, 0.0, -1.0,0.0, 0.0,
		-0.5, 0.5, -0.5,0.0, 0.0, -1.0,1.0, 0.0,
		0.5, -0.5, -0.5,0.0, -1.0, 0.0,0.0, 0.0,
		0.5, -0.5, 0.5,0.0, -1.0, 0.0,0.0, 1.0,
		-0.5, -0.5, 0.5,0.0, -1.0, 0.0,1.0, 1.0,
		-0.5, -0.5, -0.5,0.0, -1.0, 0.0,1.0, 0.0,
		-0.5, -0.5, 0.5,-1.0, 0.0, 0.0,0.0, 0.0,
		-0.5, 0.5, 0.5,-1.0, 0.0, 0.0,0.0, 1.0,
		-0.5, 0.5, -0.5,-1.0, 0.0, 0.0,1.0, 1.0,
		-0.5, -0.5, -0.5,-1.0, 0.0, 0.0,1.0, 0.0,
		0.5, -0.5, -0.5,1.0, 0.0, 0.0,0.0, 0.0,
		0.5, 0.5, -0.5,1.0, 0.0, 0.0,0.0, 1.0,
		0.5, 0.5, 0.5,1.0, 0.0, 0.0,1.0, 1.0,
		0.5, -0.5, 0.5,1.0, 0.0, 0.0,1.0, 0.0,
	};

	int indexes[] = { 0,3,2,0,1,3, 8,5,4,8,9,5, 10,7,6,10,11,7, 12,14,13,12,15,14, 16,18,17, 16,19,18,  
		20,22,21,20,23, 22};


	int vertexLength = 24;
	for (int i = 0; i < vertexLength; i++)
	{
		positions.push_back(Vector3(verticesData0[8 * i + 0], verticesData0[8 * i + 1], verticesData0[8 * i + 2]));
		normals.push_back(Vector3(verticesData0[8 * i + 3], verticesData0[8 * i + 4], verticesData0[8 * i + 5]));
		uvs.push_back(Vector2(verticesData0[8 * i + 6], verticesData0[8 * i + 7]));
	}

	int indexCount = 36;
	for (int i = 0; i < indexCount; i++)
	{
		triangles.push_back(indexes[i]);
	}

	CreateBufferData();

}

QuadMesh::QuadMesh()
{
	positions.push_back(Vector3(0.5f, 0.5f, -1.0f));
	positions.push_back(Vector3(0.5f, -0.5f, -1.0f));
	positions.push_back(Vector3(-0.5f, -0.5f, -1.0f));
	positions.push_back(Vector3(-0.5f, 0.5f, -1.0f));

	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

	colors.push_back(Color(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(Color(0.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(Color(0.0f, 0.0f, 1.0f, 1.0f));
	colors.push_back(Color(1.0f, 1.0f, 0.0f, 1.0f));

	uvs.push_back(Vector2(1.0f, 1.0f));
	uvs.push_back(Vector2(1.0f, 0.0f));
	uvs.push_back(Vector2(0.0f, 0.0f));
	uvs.push_back(Vector2(0.0f, 1.0f));

	triangles.push_back(0);
	triangles.push_back(1);
	triangles.push_back(2);
	triangles.push_back(0);
	triangles.push_back(2);
	triangles.push_back(3);

	CreateBufferData();
}


SphereMesh::SphereMesh(int row, int col)
{
	int m = row; //row 
	int n = col; //col 
	float width = 8;
	float height = 6;
	positions.resize((m + 1) * (n + 1));//the positions of vertices 
	//spheres = new GameObject[(m + 1) * (n + 1)];
	uvs.resize((m + 1) * (n + 1));
	//normals = new Vector3[(m + 1) * (n + 1)];
	triangles.resize(6 * m * n);

	for (int i = 0; i < positions.size(); i++)
	{
		float x = i % (n + 1);
		float y = i / (n + 1);
		float x_pos = x / n * width;
		float y_pos = y / m * height;
		positions[i] = Vector3(x_pos, y_pos, 0);

		float u = x / n;
		float v = y / m;
		uvs[i] = Vector2(u, v);
	}

	for (int i = 0; i < 2 * m * n; i++)
	{
		int triIndex[3];
		if (i % 2 == 0)
		{
			triIndex[0] = i / 2 + i / (2 * n);
			triIndex[1] = triIndex[0] + 1;
			triIndex[2] = triIndex[0] + (n + 1);
		}
		else
		{
			triIndex[0] = (i + 1) / 2 + i / (2 * n);
			triIndex[1] = triIndex[0] + (n + 1);
			triIndex[2] = triIndex[1] - 1;

		}
		triangles[i * 3] = triIndex[0];
		triangles[i * 3 + 1] = triIndex[1];
		triangles[i * 3 + 2] = triIndex[2];
	}

	int r = 1;
	for (int i = 0; i < positions.size(); i++)
	{
		Vector3 v;
		v.x = r * Mathf::Cos(positions[i].x / width * 2 * Mathf::Pi) * Mathf::Cos(positions[i].y / height * Mathf::Pi - Mathf::Pi / 2);
		v.y = r * Mathf::Sin(positions[i].x / width * 2 * Mathf::Pi) * Mathf::Cos(positions[i].y / height * Mathf::Pi - Mathf::Pi / 2);
		v.z = r * Mathf::Sin(positions[i].y / height * Mathf::Pi - Mathf::Pi / 2);
		//v = vertices[i];
		positions[i] = v;
	}

	CreateBufferData();
}
