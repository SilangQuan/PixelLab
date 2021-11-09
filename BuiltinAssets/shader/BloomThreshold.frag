#version 400

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sceneColorRT;
uniform float bloomThreshold;
uniform float bloomIntensity;

void main()
{             
    vec3 hdrColor = texture(sceneColorRT, TexCoords).rgb;
    vec3 color;
    color.rgb = max(hdrColor.rgb - vec3(bloomThreshold), vec3(0.0));

	color = color * bloomIntensity;
    FragColor = vec4(color, 1.0);
}