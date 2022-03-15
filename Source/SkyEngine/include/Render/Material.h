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

enum MaterialFlags
{
	EMaterialFlags_CastShadow = 0x1,
	EMaterialFlags_ReceiveShadow = 0x2,
	EMaterialFlags_Transparent = 0x4,
};

struct MaterialDescription final
{
	string name;
	string shader;

	Vector4 emissiveColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	Vector4 albedoColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	// UV anisotropic roughness (isotropic lighting models use only the first value). ZW values are ignored
	Vector4 roughness = { 1.0f, 1.0f, 0.0f, 0.0f };
	float transparencyFactor = 1.0f;
	float alphaTest = 0.0f;
	float metallicFactor = 0.0f;
	uint32 flags = 0;
	uint32 cullMode = 2;
	uint32 fillMode = 2;
	uint32 zTest = 4;
	uint32 zWrite = 0;

	string ambientOcclusionMap;
	string emissiveMap;
	string albedoMap;

	/// Occlusion (R), Roughness (G), Metallic (B) https://github.com/KhronosGroup/glTF/issues/857
	string metallicRoughnessMap;
	string normalMap;
	string opacityMap;

};


class Material
{
private:
	ShaderProgram* mShaderProgram;
	vector<TextureVariable*> mTextures;
public:
	ECullMode CullMode;
	EFillMode FillMode;
	EZTestMode ZTestMode;
	EZWriteMode ZWriteMode;

	Material(ShaderProgram* shader);
	Material(const MaterialDescription& description, string sceneName);
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

	string name;
};

