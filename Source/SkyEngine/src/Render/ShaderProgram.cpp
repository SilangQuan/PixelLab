#include "Render/ShaderProgram.h"
#include "Render/RenderContext.h"

bool EndsWith(std::string const& fullString, std::string const& ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}

//One global pointer to the currently bound shader
ShaderProgram* CURRENT_SHADER = 0;

ShaderProgram* getCurrentShader()
{
	return CURRENT_SHADER;
}

ShaderProgram::ShaderProgram(const std::string& vsFile, const std::string& fsFile)
	: m_programID(-1), m_uniforms()
{
	if (!EndsWith(vsFile, "vert"))
	{
		qDebug() << "Error: Only accept vertex shader file ends with .vert File.";
	}

	if (!EndsWith(fsFile, "frag"))
	{
		qDebug() << "Error: Only accept fragment shader file ends with .frag File.";
	}

	mFilePath = vsFile + fsFile;
	for (uint32 i = 0; i < Shader::NUM_SHADER_TYPES; i++)
	{
		shaders[i] = 0;
	}
	this->init(vsFile, fsFile);
}

ShaderProgram::ShaderProgram(const std::string& csFile)
	: m_programID(-1), m_uniforms()
{
	if (!EndsWith(csFile, "compute"))
	{
		qDebug() << "Error: Only accept cs file ends with .compute.";
		return;
	}

	mFilePath = csFile;
	
	for (uint32 i = 0; i < Shader::NUM_SHADER_TYPES; i++)
	{
		shaders[i] = 0;
	}
	this->init(csFile);
}

ShaderProgram::ShaderProgram(ShaderProgram& shaderProgram)
{
	m_programID = glCreateProgram();

	for (uint32 i = 0; i < Shader::NUM_SHADER_TYPES; i++)
	{
		shaders[i] = 0;
	}

	for (uint32 i = 0; i < Shader::NUM_SHADER_TYPES; i++)
	{
		Shader* shader = shaderProgram.shaders[i];
		if (shader != 0)
		{
			addShader(shader);
		}
	}
	this->link();
	this->validate();
	this->detectUniforms();
}

void ShaderProgram::Use()
{
	glUseProgram(m_programID);
}

void ShaderProgram::Dispatch(unsigned int x, unsigned int y, unsigned int z) const {
	glDispatchCompute(x, y, z);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ShaderProgram::Bind(RenderContext* renderContext)
{
	CURRENT_SHADER = this;

	//Bind the program
	glUseProgram(m_programID);

	if (viewPosUniform != 0)
	{
		viewPosUniform->setData(renderContext->viewPos);
	}

	if (viewUniform != 0)
	{
		viewUniform->setData(renderContext->viewMatrix);
	}

	if (projectionUniform != 0)
	{
		projectionUniform->setData(renderContext->projectionMatrix);
	}

	if (modelUniform != 0)
	{
		modelUniform->setData(renderContext->modelMatrix);
	}

	if (viewProjectionUniform != 0)
	{
		viewProjectionUniform->setData(renderContext->viewProjectionMatrix);
	}

	//Bind all the uniforms
	vector<IUniform*>::iterator i;
	for (i = m_uniforms.begin(); i != m_uniforms.end(); i++)
	{
		(*i)->bind();
	}
}


//Used for deferred rendering
void ShaderProgram::Bind()
{
	CURRENT_SHADER = this;
	//Do not Use here
	//glUseProgram(m_programID);

	//Bind all the uniforms
	vector<IUniform*>::iterator i;
	for (i = m_uniforms.begin(); i != m_uniforms.end(); i++)
	{
		(*i)->bind();
	}
}

GLuint ShaderProgram::GetProgramID() const
{
	return m_programID;
}

uint32 ShaderProgram::GetNumUniforms() const
{
	return m_uniforms.size();
}

//A linear search to find an IUniform with a particular name. Returns m_uniforms.size() if none found
uint32 ShaderProgram::FindUniform(const string& uniformName) const
{
	uint32 index;
	for (index = 0; index < m_uniforms.size(); index++)
	{
		const string &currentName = m_uniforms[index]->getName();
		if (currentName.compare(uniformName) == 0)
		{
			return index;
		}
	}
	return index;
}

bool ShaderProgram::HasUniform(const string& uniformName) const
{
	return this->FindUniform(uniformName) != GetNumUniforms();
}

IUniform* ShaderProgram::GetUniform(const string& uniformName)
{
	/*auto iter = uniformsMap.find(uniformName);
	if (iter != uniformsMap.end())
	{
		return uniformsMap[uniformName];
	}
	else
	{
		return 0;
	}*/
	//return uniformsMap[uniformName];
	uint32 index = FindUniform(uniformName);
	if (index < m_uniforms.size())
	{
		return m_uniforms[index];
	}
	else
	{
		return 0;
	}
}


void ShaderProgram::init(const std::string& csFile)
{
	this->create();

	Shader* csShader = ResourceManager::GetInstance()->TryGetResource<Shader>(csFile);

	this->addShader(csShader);

	this->link();
	this->validate();
	this->detectUniforms();
}


void ShaderProgram::init(const std::string& vsFile, const std::string& fsFile)
{
	this->create();

	Shader* vsShader = ResourceManager::GetInstance()->TryGetResource<Shader>(vsFile);
	Shader* fsShader = ResourceManager::GetInstance()->TryGetResource<Shader>(fsFile);

	this->addShader(vsShader);
	this->addShader(fsShader);

	this->link();
	this->validate();
	this->detectUniforms();
}


//Returns the address of the shader just added
Shader* ShaderProgram::addShader(Shader* shader)
{
	Shader::ShaderType shaderType = shader->type;

	shaders[shaderType] = shader;

	return shader;
}

void ShaderProgram::create()
{
	m_programID = glCreateProgram();
}

void ShaderProgram::link()
{
	//Attach all of our shaders to the program object
	for (uint32 i = 0; i < Shader::NUM_SHADER_TYPES; i++)
	{
		Shader* shader = shaders[i];
		if (shader != 0)
		{
			glAttachShader(m_programID, shader->shaderID);
		}
	}

	//And link
	glLinkProgram(m_programID);

	//Throw exception if failed
	GLint linkStatus;
	glGetProgramiv(m_programID, GL_LINK_STATUS, &linkStatus);

	if (linkStatus != GL_TRUE)
	{
		qDebug() << "ShaderProgram Link error: " << mFilePath;
	}
}

void ShaderProgram::validate()
{
	glValidateProgram(m_programID);

	GLint validateStatus;
	glGetProgramiv(m_programID, GL_LINK_STATUS, &validateStatus);

	if (validateStatus != GL_TRUE)
	{
		qDebug() << "ShaderProgram validate error: " << mFilePath;
	}
}

void ShaderProgram::detectUniforms()
{
	GLint numActiveUniforms;
	glGetProgramiv(m_programID, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

	for (uint32 i = 0; i < (uint32)numActiveUniforms; i++)
	{
		GLint nameLength;
		glGetActiveUniformsiv(m_programID, 1, &i, GL_UNIFORM_NAME_LENGTH, &nameLength);

		GLchar* uniformName = new GLchar[nameLength];
		glGetActiveUniformName(m_programID, i, nameLength, 0, uniformName);

		GLint uniformType;
		glGetActiveUniformsiv(m_programID, 1, &i, GL_UNIFORM_TYPE, &uniformType);

		GLenum type = (GLenum)uniformType;

		switch (type)
		{
		case GL_FLOAT:
			this->AddUniform<float>(uniformName);
			break;
		case GL_FLOAT_VEC2:
			this->AddUniform<Vector2>(uniformName);
			break;
		case GL_FLOAT_VEC3:
			this->AddUniform<Vector3>(uniformName);
			break;
		case GL_FLOAT_VEC4:
			this->AddUniform<Vector4>(uniformName);
			break;
		case GL_DOUBLE:
			this->AddUniform<double>(uniformName);
			break;
		case GL_INT:
			this->AddUniform<int>(uniformName);
			break;
		case GL_UNSIGNED_INT:
			this->AddUniform<uint32>(uniformName);
			break;
		case GL_BOOL:
			this->AddUniform<bool>(uniformName);
			break;
		case GL_FLOAT_MAT2:
			this->AddUniform<Matrix2>(uniformName);
			break;
		case GL_FLOAT_MAT3:
			this->AddUniform<Matrix3>(uniformName);
			break;
		case GL_FLOAT_MAT4:
			this->AddUniform<Matrix4x4>(uniformName);
			break;
		case GL_SAMPLER_1D:
			break;
		case GL_SAMPLER_2D:
			this->AddUniform<TextureVariable>(uniformName);
			break;
		case GL_SAMPLER_3D:
			this->AddUniform<TextureVariable>(uniformName);
			break;
		case GL_SAMPLER_CUBE:
			this->AddUniform<TextureVariable>(uniformName);
			break;
		default:
			break;
		}

		delete uniformName;
	}

	viewPosUniform = TryGetUniform<Vector3>("viewPos");
	viewUniform = TryGetUniform<Matrix4x4>("view");
	modelUniform = TryGetUniform<Matrix4x4>("model");
	projectionUniform = TryGetUniform<Matrix4x4>("projection");
	viewProjectionUniform = TryGetUniform<Matrix4x4>("viewProjection");
}

void ShaderProgram::destroy()
{
	//Destroy all the Shaders
	for (uint32 i = 0; i < Shader::NUM_SHADER_TYPES; i++)
	{
		Shader* shader = shaders[i];
		if (shader != 0)
		{
			delete shader;
		}
	}

	//Destroy all the UniformVariables
	for (uint32 i = 0; i < m_uniforms.size(); i++)
	{
		IUniform* uniform = m_uniforms[i];
		if (uniform != 0)
		{
			delete uniform;
		}
	}

	m_uniforms.clear();

	//Destroy the program object
	glDeleteProgram(m_programID);
}

ShaderProgram::~ShaderProgram()
{
	this->destroy();
}


void ShaderProgram::SetDirectionLightUniform(Light& light)
{
	glUniform3f(glGetUniformLocation(m_programID, "dirLight.direction"), light.direction.x, light.direction.y, light.direction.z);
	glUniform3f(glGetUniformLocation(m_programID, "dirLight.color"), light.color.r, light.color.g, light.color.b);
}

void ShaderProgram::SetPointLightsUniform(vector<Light>& light)
{
	for (vector<Light>::const_iterator lightIter = light.begin(); lightIter != light.end(); lightIter++)
	{
		glUniform3f(glGetUniformLocation(m_programID, "pointLights[0].position"), lightIter->position.x, lightIter->position.y, lightIter->position.z);
		glUniform3f(glGetUniformLocation(m_programID, "pointLights[0].color"), lightIter->color.r, lightIter->color.g, lightIter->color.b);
		glUniform1f(glGetUniformLocation(m_programID, "pointLights[0].constant"), lightIter->constant);
		glUniform1f(glGetUniformLocation(m_programID, "pointLights[0].linear"), lightIter->linear);
		glUniform1f(glGetUniformLocation(m_programID, "pointLights[0].quadratic"), lightIter->quadratic);
	}
}

void ShaderProgram::BindTextureVariable(TextureVariable* texture)
{
	IUniform* iuniform = this->GetUniform(texture->GetUniformName());

	if (iuniform == 0)
	{
		string message = " Error:This ShaderProgram :";
		message += mFilePath;
		message += " does not have an IUniform with name '";
		message += texture->GetUniformName();
		message += "'.";
		qDebug() << message;
		return;
	}

	if (iuniform == NULL)
	{
		string message = "Invalid type conversion for UniformVariable<T> with name '";
		message += texture->GetUniformName();
		message += "'.";
		qDebug() << message;
	}
	else
	{
		glUniform1i(iuniform->getLocation(), texture->GetTextureUnit());
		texture->bind();
	}
}