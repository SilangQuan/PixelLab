#pragma once
#include "EngineBase.h"
#include "Core/Resource.h"

class Shader : public Resource
{
public:
	enum ShaderType
	{
		VERTEX_SHADER,
		TESS_CONTROL_SHADER,
		TESS_EVALUATION_SHADER,
		GEOMETRY_SHADER,
		FRAGMENT_SHADER,
		COMPUTE_SHADER,
		NUM_SHADER_TYPES
	};
	Shader(const std::string &file);
	Shader(const std::string& filePath, ShaderType type);
	~Shader();

	GLuint shaderID;
	ShaderType type;
	std::string shaderName;
	
	void Init(const std::string& filePath);
	void Create();
	static std::string LoadShaderSource(const std::string& filePath);
	void CompileShaderFromSource(const std::string& source);
	void Destroy();

	inline bool EndsWith(std::string const & value, std::string const & ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}
};

