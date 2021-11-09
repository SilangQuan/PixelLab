#version 400

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBuffer;

uniform float exposure;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 filmicTonemapping(vec3 x)
{
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
float vignette(vec2 pos, float inner, float outer)
{
    float r = length(pos);
    r = 1.0 - smoothstep(inner, outer, r);
    return r;
}


void main()
{             
    const float gamma = 1.0/2.2;

    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    hdrColor += texture(bloomBuffer, TexCoords).rgb;
    vec3 color;

    color = filmicTonemapping(exposure * hdrColor);

    color.r = pow(color.r, gamma);
    color.g = pow(color.g, gamma);
    color.b = pow(color.b, gamma);

    color = color * vignette(TexCoords*2.0-1.0, 0.85, 1.7);

    FragColor = vec4(color, 1.0);
   
}