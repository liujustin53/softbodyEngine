#version 410 core
// From Vertex Buffer Object (VBO)
// The only thing that can come 'in', that is
// what our shader reads, the first part of the
// graphics pipeline.
layout(location=0) in vec3 position;
layout(location=1) in vec2 textures;
layout(location=2) in vec3 normals;

// Uniform variables
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_LightSpaceMatrix;

out vec3 fragPos;
out vec2 texCoord;
out vec3 vertexNormal;
out vec4 fragPosLightSpace;

void main()
{
  fragPos = vec3(u_Model * vec4(position, 1.0));
  texCoord = textures;
  vertexNormal = mat3(transpose(inverse(u_Model))) * normals;
  fragPosLightSpace = u_LightSpaceMatrix * vec4(fragPos, 1.0);

  gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0);
}
