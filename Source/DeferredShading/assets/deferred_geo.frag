#version 330
in vec3 fsPos;
in vec3 fsNormal;
in vec2 fsUv;
uniform sampler2D uDiffTex;
out vec4 geoData[3];
void main()
{
    vec4 diff = texture(uDiffTex, vec2(1.0, -1.0)*fsUv).rgba;
    if (diff.a < 0.2)
    {
        discard;
    }
    geoData[0] = vec4(diff.rgb, 1);
    geoData[1] = vec4(fsNormal, 1);
    geoData[2] = vec4(fsPos, 1);
}
