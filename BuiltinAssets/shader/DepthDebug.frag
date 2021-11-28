#version 430 core

struct View
{
	mat4 View;
	mat4 Projection;
	mat4 InvProjection;
	vec4 ScreenSizeAndInv;
	vec4 ZParams;
};

uniform View ViewInfo;

uniform sampler2D DepthRT;
out vec4 fragColor;
in vec2 texCoord0;

// convert a depth value from post-projection space into view space
float ConvertProjDepthToView(float z)
{
	//return -ViewInfo.InvProjection[3][2] /(ViewInfo.InvProjection[2][2] + (2 * z - 1));
	
	
	//z = 1.f / (z * ViewInfo.InvProjection[2][3] + ViewInfo.InvProjection[3][3]);
	//return z;

	z = 1.f / (ViewInfo.ZParams.y - z*ViewInfo.ZParams.x);
	return z;
}
void main()
{
	float Depth = texture(DepthRT, texCoord0).r;
	float linearZ = ConvertProjDepthToView(Depth);
	fragColor = vec4(linearZ);
}