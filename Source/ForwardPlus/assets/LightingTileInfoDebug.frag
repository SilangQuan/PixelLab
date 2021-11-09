#version 430 core
#define TILE_SIZE 16
#define MAX_LIGHTS_PER_TILE 1024

struct Light {
	vec3 position;
	float radius;
	vec3 color;
	float intensity;
};

layout (std430, binding = 0) readonly buffer lights_data { 
	Light lights[];
};

layout(std430, binding = 1) writeonly buffer visible_lights_indices {
	int lights_indices[];
};


out vec4 fragColor;

void main()
{
	fragColor = vec4(1,0,0,1);
}