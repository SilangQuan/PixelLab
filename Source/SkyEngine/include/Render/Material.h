#pragma once
#include "Base.h"
class ShaderProgram;
class TextureVariable;
class Texture;
class Light;
class Camera;
class RenderContext;

enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
enum EPbrWorkflow
{
	METALLIC_ROUGHNESS,
	SPECULAR_GLOSSINESS
};

struct TfMaterial {

	AlphaMode alphaMode = ALPHAMODE_OPAQUE;

	EPbrWorkflow workflow = METALLIC_ROUGHNESS;
	float alphaCutoff = 1.0f;
	float metallicFactor = 1.0f;
	float roughnessFactor = 1.0f;
	Color baseColorFactor = Color::white;
	Color emissiveFactor = Color::white;
	TextureVariable* baseColorTexture;
	TextureVariable* metallicRoughnessTexture;
	TextureVariable* normalTexture;
	TextureVariable* occlusionTexture;
	TextureVariable* emissiveTexture;
	string name;
	struct TexCoordSets {
		uint8_t baseColor = 0;
		uint8_t metallicRoughness = 0;
		uint8_t specularGlossiness = 0;
		uint8_t normal = 0;
		uint8_t occlusion = 0;
		uint8_t emissive = 0;
	} texCoordSets;

	struct Extension {
		TextureVariable* specularGlossinessTexture;
		TextureVariable* diffuseTexture;
		Vector4 diffuseFactor = Vector4::one;
		Vector3 specularFactor = Vector3::zero;
	} extension;
};


class Material
{
private:
	ShaderProgram* shaderProgram;

public:

	ECullMode CullMode;
	EFillMode FillMode;
	EZTestMode ZTestMode;
	EZWriteMode ZWriteMode;

	Material(ShaderProgram* shader);
	virtual ~Material();

	void Bind(RenderContext* renderContext);
	void Bind();

	void SetCullMode(ECullMode mode);
	void SetFillMode(EFillMode mode);
	ECullMode GetCullMode() { return CullMode; };
	EFillMode GetFillMode() { return FillMode; };

	ShaderProgram* GetShaderProgram();

	void SetShader(ShaderProgram* shader);
	//void AddTexture(Texture& texture);
	void AddTextureVariable(string shaderRefName, Texture* texture, ETextureVariableType tvType = ETextureVariableType::TV_2D, int textureUnit = 0);
	void AddTextureVariable(TextureVariable* texture);


	void SetFloat(string shaderRefName, float value);
	void SetVector3(string shaderRefName, Vector3 value);
	void SetVector4(string shaderRefName, Vector4 value);

	void SetColor(string shaderRefName, Color& c);


	void SetDirectionLight(Light& light);
	void SetPointLight(vector<Light*>& light);
	void SetSpotLight(vector<Light*>& light);

	string Name;

};

