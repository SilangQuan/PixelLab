#version 330
uniform sampler2D uColorTex;
uniform sampler2D uNormalTex;
uniform sampler2D uPositionTex;
uniform sampler2D uDepthTex;

out vec4 outColor;
in vec4 fsPos;
uniform float uLightRadius;
uniform vec3 uLightPosition;
uniform vec3 uLightColor;
uniform vec3 uCameraPos;


void main()
{
    vec2 uv = (fsPos.xy / fsPos.w) * 0.5 + 0.5;
    vec3 albedo = texture(uColorTex, uv).xyz;
    vec3 n = normalize(texture(uNormalTex, uv).xyz);
    vec3 pos = texture(uPositionTex, uv).xyz;
    vec3 lightToPosVector = pos.xyz - uLightPosition;
    float lightDist = length(lightToPosVector);
    vec3 l = -lightToPosVector / (lightDist);
    float ztest = step(0.0, uLightRadius - lightDist);
    float d = lightDist / uLightRadius;
    float attenuation = 1.0 - d;
    vec3 v = normalize(uCameraPos - pos);
    vec3 h = normalize(l + v);
    vec3 color =  uLightColor * albedo.xyz * max(0.0, dot(n.xyz, l)) +  uLightColor * 0.4 * pow(max(0.0, dot(h, n)), 12.0);
    color *= ztest * attenuation;

    outColor = vec4(color, 1.0);
}