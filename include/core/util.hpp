#pragma once

#include <glad/glad.h>
#include <string>

// vvvvvvvvvvvvvvvvvvv Error Handling Routines vvvvvvvvvvvvvvv
void GLClearAllErrors();

// Returns true if we have an error
bool GLCheckErrorStatus(const char *function, int line);

#define GLCheck(x)                                                             \
  GLClearAllErrors();                                                          \
  x;                                                                           \
  GLCheckErrorStatus(#x, __LINE__);
// ^^^^^^^^^^^^^^^^^^^ Error Handling Routines ^^^^^^^^^^^^^^^

/**
 * LoadShaderAsString takes a filepath as an argument and will read line by line
 * a file and return a string that is meant to be compiled at runtime for a
 * vertex, fragment, geometry, tesselation, or compute shader. e.g.
 *       LoadShaderAsString("./shaders/filepath");
 *
 * @param filename Path to the shader file
 * @return Entire file stored as a single string
 */
std::string loadShaderAsString(const std::string &filename);

/**
 * CompileShader will compile any valid vertex, fragment, geometry, tesselation,
 *or compute shader. e.g. Compile a vertex shader:
 *CompileShader(GL_VERTEX_SHADER, vertexShaderSource); Compile a fragment
 *shader: 	CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
 *
 * @param type We use the 'type' field to determine which shader we are going to
 *compile.
 * @param source : The shader source code.
 * @return id of the shaderObject
 */
GLuint compileShader(GLuint type, const std::string &source);

/**
 * Creates a graphics program object (i.e. graphics pipeline) with a Vertex
 * Shader and a Fragment Shader
 *
 * @param vertexShaderSource Vertex source code as a string
 * @param fragmentShaderSource Fragment shader source code as a string
 * @return id of the program Object
 */
GLuint createShaderProgram(const std::string &vertexShaderSource,
                           const std::string &fragmentShaderSource);
