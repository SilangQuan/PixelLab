#pragma once

#include "Base.h"
#include "TextureVariable.h"

class IUniform
{
protected:
	string m_uniformName;
	GLuint m_uniformLocation;

public:
	IUniform(string uniformName) : m_uniformName(uniformName), m_uniformLocation(-1) {}
	virtual ~IUniform() {}

	const virtual void bind() = 0;

	//inline void attachToShader(GLuint programID);

	inline const string& getName() const { return m_uniformName; }
	inline GLuint getLocation() const { return m_uniformLocation; }

	//For IUniform

	inline void attachToShader(GLuint programID)
	{
		m_uniformLocation = glGetUniformLocation(programID, m_uniformName.c_str());
	}

	inline bool operator<(const IUniform& right) { return m_uniformName < right.getName(); }
};

template<typename T>
class UniformVariable : public IUniform
{
	T m_uniformData;

	//Just for texture variables
	const T* uniformDataHandle;

public:
	UniformVariable(string uniformName) : IUniform(uniformName), m_uniformData() {}
	virtual ~UniformVariable() {}

	inline void setDataHandle(const T* handle);
	inline void setData(const T& data);
	inline T& getData();

	const virtual void bind();

	bool hasBeenSettle;
};

/*
//A specialization for UniformVariable<Texture>
template<>
class UniformVariable<TextureVariable> : public UniformVariable<TextureVariable*>
{
public:
	UniformVariable(const string& uniformName) : UniformVariable<TextureVariable*>(uniformName) {}
};
*/

template<typename T>
void UniformVariable<T>::setData(const T& data)
{
	hasBeenSettle = true;
	m_uniformData = data;
}

template<typename T>
void UniformVariable<T>::setDataHandle(const T* handle)
{
	hasBeenSettle = true;
	uniformDataHandle = handle;
}


template<typename T>
T& UniformVariable<T>::getData()
{
	return m_uniformData;
}

//Template Instantiations for bind() (instantiations only, see .cpp for definitions)

template<> const void UniformVariable<int>::bind();
template<> const void UniformVariable<float>::bind();
template<> const void UniformVariable<double>::bind();
template<> const void UniformVariable<unsigned int>::bind();
template<> const void UniformVariable<bool>::bind();
template<> const void UniformVariable<Vector2>::bind();
template<> const void UniformVariable<Vector3>::bind();
template<> const void UniformVariable<Vector4>::bind();
template<> const void UniformVariable<Color>::bind();
template<> const void UniformVariable<Matrix2>::bind();
template<> const void UniformVariable<Matrix3>::bind();
template<> const void UniformVariable<Matrix4x4>::bind();
template<> const void UniformVariable<TextureVariable>::bind();
