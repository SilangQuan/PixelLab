#version 330
in vec2 fsUv;
out vec4 outColor;
uniform sampler2D uColorTex;
uniform sampler2D uNormalTex;
uniform sampler2D uPositionTex;
uniform sampler2D uDepthTex;
uniform vec3 uCameraPos;
uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform float uRenderType;

void main()
{
    vec3 albedo = texture(uColorTex, fsUv).xyz;
    vec3 n = normalize(texture(uNormalTex, fsUv).xyz);
    vec3 pos = texture(uPositionTex, fsUv).xyz;
    vec3 l = normalize(uLightDir);
    vec3 v = normalize(uCameraPos - pos);
    vec3 h = normalize(l + v);
    vec3 color =  uLightColor * (0.5 * albedo.xyz * max(0.0, dot(n.xyz, l)) +  0.4 * pow(max(0.0, dot(h, n)), 32.0) +  0.5 * albedo.xyz);
	if(uRenderType == 1.0)
	{
		color = albedo;
	}
	if(uRenderType == 2)
	{
		color = n;
	}
	if(uRenderType == 3)
	{
		color = pos;
	}
	if(uRenderType == 4)
	{
		vec3 depth = texture(uPositionTex, fsUv).xyz;
		color = depth;
	}
    outColor = vec4(color, 1.0);
}
