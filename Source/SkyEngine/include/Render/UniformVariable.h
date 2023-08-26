#pragma once

#include "EngineBase.h"
#include "TextureVariable.h"
class RenderDevice;

class IUniform
{
protected:
	string m_uniformName;
	uint32 m_uniformLocation;

public:
	IUniform(string uniformName) : m_uniformName(uniformName), m_uniformLocation(-1) {}
	virtual ~IUniform() {}

	const virtual void bind(const RenderDevice* device) = 0;


	inline const string& getName() const { return m_uniformName; }
	inline uint32 getLocation() const { return m_uniformLocation; }

	//For IUniform
	inline void attachToShader(uint32 programID)
	{
		m_uniformLocation = glGetUniformLocation(programID, m_uniformName.c_str());
	}

	inline bool operator<(const IUniform& right) { return m_uniformName < right.getName(); }
};

template<typename T>
class UniformVariable : public IUniform
{
	T m_uniformData;

public:
	UniformVariable(string uniformName) : IUniform(uniformName), m_uniformData(){}
	virtual ~UniformVariable() {}

	inline void setData(const T& data);
	inline T& getData();

	const virtual void bind(const RenderDevice* device);

	bool hasBeenSettle;
};


template<typename T>
void UniformVariable<T>::setData(const T& data)
{
	hasBeenSettle = true;
	m_uniformData = data;
}

template<typename T>
T& UniformVariable<T>::getData()
{
	return m_uniformData;
}

//Template Instantiations for bind() (instantiations only, see .cpp for definitions)

template<> const void UniformVariable<int>::bind(const RenderDevice* device);
template<> const void UniformVariable<float>::bind(const RenderDevice* device);
template<> const void UniformVariable<double>::bind(const RenderDevice* device);
template<> const void UniformVariable<unsigned int>::bind(const RenderDevice* device);
template<> const void UniformVariable<bool>::bind(const RenderDevice* device);
template<> const void UniformVariable<Vector2>::bind(const RenderDevice* device);
template<> const void UniformVariable<Vector3>::bind(const RenderDevice* device);
template<> const void UniformVariable<Vector4>::bind(const RenderDevice* device);
template<> const void UniformVariable<Color>::bind(const RenderDevice* device);
template<> const void UniformVariable<Matrix2>::bind(const RenderDevice* device);
template<> const void UniformVariable<Matrix3>::bind(const RenderDevice* device);
template<> const void UniformVariable<Matrix4x4>::bind(const RenderDevice* device);
template<> const void UniformVariable<TextureVariable>::bind(const RenderDevice* device);
