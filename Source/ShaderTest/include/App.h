#pragma once
#include "../../LibAppFramework/include/application.h"

static const GLchar* compute_source_edge_filter[] =
{
	"#version 430 core\n"
	"layout (local_size_x = 1024) in;\n"
	"layout (rgba32f, binding = 0) uniform image2D input_image;\n"
	"layout (rgba32f, binding = 1) uniform image2D output_image;\n"
	"shared vec4 scanline[1024];\n"
	"void main(void)\n"
	"{\n"
	"ivec2 pos = ivec2(gl_GlobalInvocationID.xy);\n"
	"scanline[pos.x] = imageLoad(input_image, pos);\n"
	"barrier();\n"
	"vec4 result = scanline[min(pos.x + 1, 1023)] -\n"
	"scanline[max(pos.x - 1, 0)];"
	"imageStore(output_image, pos.yx, result);"
	"}"
};


class App : public Application
{
public:
	//bool Initialize();
	bool CreateWorld();
	void DestroyWorld();

	void FrameMove();
	void RenderWorld();

private:
	GLuint refractionShader;
	GLuint shaderProgram;
	GLuint shaderProgram2;
	GLuint VBOs[2], VAOs[2], EBO;

	int compute_shader_obj;
	GLuint compute_shader_prog;

	GLuint input_image;
	GLuint intermediate_image;
	GLuint out_put_image;



};