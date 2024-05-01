#version 410 core

in vec2 texCoord;

out vec4 color;

uniform sampler2D u_DepthMap;

float linearizeDepth(float depth)
{
    float near = 1.0;
    float far = 100.0;
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    float depthValue = texture(u_DepthMap, texCoord).r;
    // color = vec4(vec3(linearizeDepth(depthValue) / 100.0), 1.0);
    color = vec4(vec3(depthValue), 1.0);
}
