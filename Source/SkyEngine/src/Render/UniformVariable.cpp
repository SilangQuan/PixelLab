
#include "Render/UniformVariable.h"
#include "Render/TextureVariable.h"
#include "Render/Texture.h"

template<>
const void UniformVariable<int>::bind()
{
	glUniform1i(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<float>::bind()
{
	glUniform1f(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<double>::bind()
{
	glUniform1d(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<unsigned int>::bind()
{
	glUniform1ui(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<bool>::bind()
{
	glUniform1i(m_uniformLocation, m_uniformData);
}

template<>
const void UniformVariable<Vector2>::bind()
{
	glUniform2f(m_uniformLocation,
		m_uniformData.x,
		m_uniformData.y);
}

template<>
const void UniformVariable<Vector3>::bind()
{
	glUniform3f(m_uniformLocation,
		m_uniformData.x,
		m_uniformData.y,
		m_uniformData.z);
}

template<>
const void UniformVariable<Vector4>::bind()
{
	glUniform4f(m_uniformLocation,
		m_uniformData.x,
		m_uniformData.y,
		m_uniformData.z,
		m_uniformData.w);
}

template<>
const void UniformVariable<Color>::bind()
{
	glUniform4f(m_uniformLocation,
		m_uniformData.r,
		m_uniformData.g,
		m_uniformData.b,
		m_uniformData.a);
}

template<>
const void UniformVariable<Matrix2>::bind()
{
	glUniformMatrix2fv(m_uniformLocation,
		1, false,
		&(m_uniformData[0]));
}

template<>
const void UniformVariable<Matrix3>::bind()
{
	glUniformMatrix3fv(m_uniformLocation,
		1, false,
		&(m_uniformData[0]));
}

template<>
const void UniformVariable<Matrix4x4>::bind()
{
	glUniformMatrix4fv(m_uniformLocation,
		1, false,
		&(m_uniformData[0]));
}

template<>
const void UniformVariable<TextureVariable>::bind()
{
	if (hasBeenSettle)
	{
		if (uniformDataHandle != NULL)
		{
			glUniform1i(m_uniformLocation, uniformDataHandle->GetTextureUnit());
			const_cast<TextureVariable*>(uniformDataHandle)->bind();
		}
		else
		{
			glUniform1i(m_uniformLocation, m_uniformData.GetTextureUnit());
			m_uniformData.bind();
		}
	}
}