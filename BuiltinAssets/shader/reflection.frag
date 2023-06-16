#version 330 core
in vec3 ourColor;
in vec2 TexCoords;

out vec4 outColor;

in vec4 wPosition;
in vec3 wNormal;
in vec3 IncidentVector;

uniform samplerCube envMap;
uniform vec3 emission;
uniform vec4 color;

float eta=0.7;
float deta=-0.006;

float my_fresnel(vec3 I, vec3 N, float power,  float scale,  float bias)
{
    return bias + (pow(clamp(1.0 - dot(I, N), 0.0, 1.0), power) * scale);
}

void main()
{
     vec3 I = normalize(IncidentVector);
    vec3 N = normalize(wNormal);
    vec3 R = reflect(I, N);
    vec3 T1 = refract(I, N, eta);
    vec3 T2 = refract(I, N, eta+deta);
    vec3 T3 = refract(I, N, eta+2.0*deta);
    float fresnel = my_fresnel(-I, N, 4.0, 0.99, 0.1);

    vec3 Creflect = texture(envMap, R).rgb;
    vec3 Crefract;
    Crefract.r = texture(envMap, T1).r;
    Crefract.g = texture(envMap, T2).g;
    Crefract.b = texture(envMap, T3).b;


    outColor = vec4(mix(Creflect, Crefract, fresnel)+emission, 1.0);

    //Total Reflection
	//outColor = vec4(N, 1.0f);

}