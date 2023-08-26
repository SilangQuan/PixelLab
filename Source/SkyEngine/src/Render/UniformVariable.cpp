
#include "Render/UniformVariable.h"
#include "Render/TextureVariable.h"
#include "Render/Texture.h"
#include "Render/RenderDevice.h"

template<>
const void UniformVariable<int>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<float>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<double>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<unsigned int>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<bool>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<Vector2>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<Vector3>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<Vector4>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<Color>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<Matrix2>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<Matrix3>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<Matrix4x4>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<TextureVariable>::bind(const RenderDevice* device)
{
	device->BindShaderProgramParam(m_uniformLocation, m_uniformData);
}