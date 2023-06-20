#pragma once
#include "EngineBase.h"
#include "Shader.h"
#include "Lighting/Light.h"
#include "UniformVariable.h"
class RenderContext;

/*
class ViewInfo
{
public:
	Vector3 View;
	Matrix4x4 Projection;
	Matrix4x4 InvProjection;
	Vector4 ScreenSizeAndInv;
};*/

class ShaderProgram
{

public:
	ShaderProgram(const std::string& vsFile, const std::string& fsFile);
	ShaderProgram(const std::string& csFile);
	ShaderProgram(ShaderProgram& shaderProgram);
	virtual ~ShaderProgram();

	template<typename T> inline void AddUniform(const string& uniformName, const T& uniformData);
	template<typename T> inline void SetUniform(const string& uniformName, const T& uniformData);
	template<typename T> inline void SetUniformHandle(const string& uniformName, const T* uniformDataHandle);

	template<typename T> inline void SetUniformAndBind(const string& uniformName, const T& uniformData);

	void BindTextureVariable(TextureVariable* texture);
	//template<typename T> inline void SetTestUniform(const string& uniformName, const T& uniformData);
	template<typename T> inline UniformVariable<T>* TryGetUniform(const string& uniformName);

	void Bind(RenderContext* renderContext);
	void Bind();


	void Use();
	void Dispatch(unsigned int x, unsigned int y, unsigned int z) const;
	GLuint GetProgramID() const;
	uint32 GetNumUniforms() const;
	uint32 FindUniform(const string& uniformName) const;
	bool HasUniform(const string& uniformName) const;
	IUniform* GetUniform(const string& uniformName);

	void SetDirectionLightUniform(Light& light);
	void SetPointLightsUniform(vector<Light>& light);

	UniformVariable<Matrix4x4>* modelUniform;
	UniformVariable<Matrix4x4>* viewUniform;
	UniformVariable<Vector3>* viewPosUniform;
	UniformVariable<Matrix4x4>* projectionUniform;
	UniformVariable<Matrix4x4>* viewProjectionUniform;


protected:
	virtual void init(const std::string& vsFile, const std::string& fsFile);
	void init(const std::string& csFile);

	Shader* addShader(Shader* shader);
	void create();
	void link();
	void validate();
	void detectUniforms();
	template<typename T> inline void AddUniform(const string& uniformName);
	void destroy();

private:
	GLuint m_programID;
	Shader* shaders[Shader::NUM_SHADER_TYPES]; //ShaderProgram has ownership of its shaders
	map<const string, IUniform*> uniformsMap;
	vector<IUniform*> m_uniforms;	//ShaderProgram does not have ownership of its UniformVariables

	bool hasViewPosUniform;
	bool hasViewUniform;
	bool hasModelUniform;
	bool hasProjectionUniform;
	bool hasViewProjectionUniform;
	
	string mFilePath;

	//ViewInfo mViewInfo;
};


template<typename T>
void ShaderProgram::AddUniform(const string& uniformName)
{
	UniformVariable<T>* uniform = new UniformVariable<T>(uniformName);
	uniform->attachToShader(this->GetProgramID());
	m_uniforms.push_back(uniform);
	uniformsMap.insert(make_pair(uniformName, uniform));
}

template<typename T>
void ShaderProgram::AddUniform(const string& uniformName, const T& uniformData)
{
	UniformVariable<T>* uniform = new UniformVariable<T>(uniformName);
	uniform->attachToShader(this->GetProgramID());
	uniform->setData(uniformData);

	m_uniforms.push_back(uniform);
	uniformsMap.insert(make_pair(uniformName, uniform));
}
template<typename T> 
void ShaderProgram::SetUniformHandle(const string& uniformName, const T* uniformDataHandle)
{
	IUniform* iuniform = this->GetUniform(uniformName);

	if (iuniform == 0)
	{
		string message = " Error:This ShaderProgram :";
		message += mFilePath;
		message += " does not have an IUniform with name '";
		message += uniformName;
		message += "'.";
		qDebug() << message;
		return;
	}

	UniformVariable<T>* uniform = NULL;
	try {
		uniform = static_cast<UniformVariable<T>*>(iuniform);
		//uniform = dynamic_cast<UniformVariable<T>*>(iuniform);
		//uniformHandler = static_cast<UniformVariable<T*> >(iuniform);
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}


	if (uniform == NULL)
	{
		string message = "Invalid type conversion for UniformVariable<T> with name '";
		message += uniformName;
		message += "'.";
		qDebug() << message;
	}
	else
	{
		uniform->setDataHandle(uniformDataHandle);
	}
}




template<typename T>
void ShaderProgram::SetUniformAndBind(const string& uniformName, const T& uniformData)
{
	IUniform* iuniform = this->GetUniform(uniformName);

	if (iuniform == 0)
	{
		string message = " Error:This ShaderProgram :";
		message += mFilePath;
		message += " does not have an IUniform with name '";
		message += uniformName;
		message += "'.";
		qDebug() << message;
		return;
	}

	UniformVariable<T>* uniform = NULL;
	try {
		//uniform = dynamic_cast<UniformVariable<T>*>(iuniform);
		uniform = static_cast<UniformVariable<T>*>(iuniform);
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}


	if (uniform == NULL)
	{
		string message = "Invalid type conversion for UniformVariable<T> with name '";
		message += uniformName;
		message += "'.";
		qDebug() << message;
	}
	else
	{
		uniform->setData(uniformData);
		uniform->bind();
	}
}


template<typename T>
void ShaderProgram::SetUniform(const string& uniformName, const T& uniformData)
{
	IUniform* iuniform = this->GetUniform(uniformName);

	if (iuniform == 0)
	{
		string message = " Error:This ShaderProgram :";
		message += mFilePath;
		message += " does not have an IUniform with name '";
		message += uniformName;
		message += "'.";
		qDebug()<<message;
		return;
	}

	UniformVariable<T>* uniform = NULL;
	try {
		//uniform = dynamic_cast<UniformVariable<T>*>(iuniform);
		uniform = static_cast<UniformVariable<T>*>(iuniform);
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
	

	if (uniform == NULL)
	{
		string message = "Invalid type conversion for UniformVariable<T> with name '";
		message += uniformName;
		message += "'.";
		qDebug() << message;
	}
	else
	{
		uniform->setData(uniformData);
	}
}

template<typename T>
UniformVariable<T>* ShaderProgram::TryGetUniform(const string& uniformName)
{
	IUniform* iuniform = this->GetUniform(uniformName);

	if (iuniform == 0)
	{
		string message = "Info:This ShaderProgram ";
		message += mFilePath;
		message += " does not have an IUniform with name '";
		message += uniformName;
		message += "'.";
		//qDebug() << message;
		return NULL;
	}

	UniformVariable<T>* uniform = dynamic_cast<UniformVariable<T>*>(iuniform);

	if (uniform == 0)
	{
		string message = "Info :Invalid type conversion for UniformVariable<T> with name '";
		message += uniformName;
		message += "'.";
		//qDebug() << message;
	}

	return uniform;
}

