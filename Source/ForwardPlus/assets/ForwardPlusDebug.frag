#version 430 core
#define TILE_SIZE 16
#define MAX_LIGHTS_PER_TILE 4

struct Light {
	vec3 position;
	float radius;
	vec3 color;
	float intensity;
};

layout (std430, binding = 0) readonly buffer lights_data { 
	Light lights[];
};

layout(std430, binding = 1) readonly buffer visible_lights_indices {
	int lights_indices[];
};

out vec4 fragColor;
uniform int workgroup_x;
uniform int workgroup_y;

void main()
{
	ivec2 loc = ivec2(gl_FragCoord.xy);
    ivec2 tileID = loc / ivec2(TILE_SIZE, TILE_SIZE);
    uint index = tileID.y * workgroup_x + tileID.x;
    uint offset = index * MAX_LIGHTS_PER_TILE;

	uint count = 0;
	for (uint i = 0; i < MAX_LIGHTS_PER_TILE; i++) 
	{
	   if (lights_indices[offset + i] != -1) 
	   {
		count++;
	   }
	}
	//float shade = float(count) / float(MAX_LIGHTS_PER_TILE * 2); 
	float shade = float(count) / MAX_LIGHTS_PER_TILE; 

	float shadeX = float(tileID.x)/workgroup_x;
	float shadeY = float(tileID.y)/workgroup_y;
	fragColor = vec4(max(shadeX, shadeY));
	//fragColor = vec4(shadeX);

	fragColor = vec4(shade);

	//shade = lights_indices[offset]/100000;
	//shade /= 200;

	fragColor = vec4(shade);
}