#version 410 core
// From Vertex Buffer Object (VBO)
// The only thing that can come 'in', that is
// what our shader reads, the first part of the
// graphics pipeline.
layout(location=0) in vec3 position;
layout(location=1) in vec2 textures;
layout(location=2) in vec3 normals;
layout(location=3) in vec3 tangents;
layout(location=4) in vec3 bitangents;

// Uniform variables
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

out vec3 fragPos;
out vec2 texCoord;
out vec3 tangentLightPos;
out vec3 tangentViewPos;
out vec3 tangentFragPos;

void main()
{
  fragPos = vec3(u_Model * vec4(position, 1.0));
  texCoord = textures;

  mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
  vec3 T = normalize(vec3(normalMatrix * tangents));
  vec3 N = normalize(vec3(normalMatrix * normals));
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);

  mat3 TBN = transpose(mat3(T, B, N));
  // Calculate the light position in tangent space
  tangentLightPos = TBN * u_LightPos;
  tangentViewPos = TBN * u_ViewPos;
  tangentFragPos = TBN * fragPos;

  gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0);
}
