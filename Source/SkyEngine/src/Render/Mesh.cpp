#include "Render/Mesh.h"
#include "Render/ShaderProgram.h"




//The drawing function that is shared between all mesh primitives
void Primitive::Draw(const unsigned int readTexture1, const unsigned int readTexture2, const unsigned int readTexture3) {
    glBindVertexArray(VAO);

    //This texture read could be compacted into a for loop and an array could be passed instead
    //But for now this is sufficient 
    if (readTexture1 != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, readTexture1);
    }

    //A texture id of 0 is never assigned by opengl so we can
    //be sure that it means we haven't set any texture in the second paramenter and therefore
    //we only want one texture
    if (readTexture2 != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, readTexture2);
    }

    if (readTexture3 != 0) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, readTexture3);
    }

    glDrawArrays(GL_TRIANGLES, 0, numVertices);

}

//----------------------------------------------------------------------------------------------------
//Quad Class stuff
void Quad::Setup() {
    const float quadVertices[] = {
        //positions //texCoordinates
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    //OpenGL postprocessing quad setup
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //Bind Vertex Array Object and VBO in correct order
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //VBO initialization
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    //Quad position pointer initialization in attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    //Quad texcoords pointer initialization in attribute array
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

//Quads never need to be depth tested when used for screen space rendering
void Quad::Draw(const unsigned int readTex1, const unsigned int readTex2, const unsigned int readTex3) {
    glDisable(GL_DEPTH_TEST);
    Primitive::Draw(readTex1, readTex2, readTex3);
}

//----------------------------------------------------------------------------------------------------
//Cube class 
void Cube::Setup() {
    const float boxVertices[108] = {
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f };

    //Generate Buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //Bind Vertex Array Object and VBO in correct order
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //VBO initialization 
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), &boxVertices, GL_STATIC_DRAW);

    //Vertex position pointer init
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //Unbinding VAO
    glBindVertexArray(0);
}


Mesh::Mesh() : positions(), normals(), uvs(), triangles(), vaoID(-1), iboID(-1)
{
	for (uint32 i = 0; i < (uint32)Mesh::NUM_VERTEX_ATTRIBUTES; i++)
	{
		vboIDs[i] = -1;
	}
}
Mesh::Mesh(vector<Vector3> inPositions, vector<Vector3> inNormals, vector<Vector2> inUv, vector<uint32> inTriangles, vector<TextureVariable*> inTextures): positions(inPositions), normals(inNormals), uvs(inUv), triangles(inTriangles),textures(inTextures), vaoID(-1), iboID(-1)
{
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
	GLfloat verticesData[] = {
		// back face
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
				// bottom face
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				// top face
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
				 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left    

	};
	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat verticesData1[] = {
		// Positions      // Normals     // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	vector<Color> colors;
	colors.push_back(Color(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(Color(0.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(Color(0.0f, 0.0f, 1.0f, 1.0f));
	colors.push_back(Color(1.0f, 1.0f, 0.0f, 1.0f));

	int vertexLength = 36;
	for (int i = 0; i < vertexLength; i++)
	{
		positions.push_back(Vector3(verticesData[8 * i + 0], verticesData[8 * i + 1], verticesData[8 * i + 2]));
		normals.push_back(Vector3(verticesData[8 * i + 3], verticesData[8 * i + 4], verticesData[8 * i + 5]));
		uvs.push_back(Vector2(verticesData[8 * i + 6], verticesData[8 * i + 7]));
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