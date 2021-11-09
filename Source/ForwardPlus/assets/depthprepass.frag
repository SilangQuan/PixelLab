#version 430 core

uniform float zNear;
uniform float zFar;

out vec4 fragColor;

// Need to linearize the depth because we are using the projection
float LinearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0;
	return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}

void main() {
	float depth = LinearizeDepth(gl_FragCoord.z) / zFar;
	fragColor = vec4(vec3(depth), 1.0f);
}