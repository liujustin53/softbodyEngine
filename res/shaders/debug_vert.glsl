#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec2 textures;

out vec2 texCoord;

void main()
{
    texCoord = textures;
    gl_Position = vec4(position, 1.0);
}
