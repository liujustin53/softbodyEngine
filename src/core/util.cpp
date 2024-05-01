#include "core/util.hpp"

#include <fstream>
#include <iostream>

std::string loadShaderAsString(const std::string &filename) {
  // Resulting shader program loaded as a single string
  std::string result = "";

  std::string line = "";
  std::ifstream myFile(filename.c_str());

  if (myFile.is_open()) {
    while (std::getline(myFile, line)) {
      result += line + '\n';
    }
    myFile.close();
  } else {
    std::cout << "Unable to open file " << filename << "\n";
  }

  return result;
}

GLuint compileShader(GLuint type, const std::string &source) {
  // Compile our shaders
  GLuint shaderObject;

  // Based on the type passed in, we create a shader object specifically for
  // that type.
  if (type == GL_VERTEX_SHADER) {
    shaderObject = glCreateShader(GL_VERTEX_SHADER);
  } else if (type == GL_FRAGMENT_SHADER) {
    shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
  }

  const char *src = source.c_str();
  // The source of our shader
  glShaderSource(shaderObject, 1, &src, nullptr);
  // Now compile our shader
  glCompileShader(shaderObject);

  // Retrieve the result of our compilation
  int result;
  // Our goal with glGetShaderiv is to retrieve the compilation status
  glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
    char *errorMessages = new char[length]; // Could also use alloca here.
    glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

    if (type == GL_VERTEX_SHADER) {
      std::cout << "ERROR: GL_VERTEX_SHADER compilation failed!\n"
                << errorMessages << "\n";
    } else if (type == GL_FRAGMENT_SHADER) {
      std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed!\n"
                << errorMessages << "\n";
    }
    // Reclaim our memory
    delete[] errorMessages;

    // Delete our broken shader
    glDeleteShader(shaderObject);

    return 0;
  }

  return shaderObject;
}

GLuint createShaderProgram(const std::string &vertexShaderSource,
                           const std::string &fragmentShaderSource) {

  // Create a new program object
  GLuint programObject = glCreateProgram();

  // Compile our shaders
  GLuint myVertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
  GLuint myFragmentShader =
      compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

  // Link our two shader programs together.
  // Consider this the equivalent of taking two .cpp files, and linking them
  // into one executable file.
  glAttachShader(programObject, myVertexShader);
  glAttachShader(programObject, myFragmentShader);
  glLinkProgram(programObject);

  // Validate our program
  glValidateProgram(programObject);

  // Once our final program Object has been created, we can
  // detach and then delete our individual shaders.
  glDetachShader(programObject, myVertexShader);
  glDetachShader(programObject, myFragmentShader);
  // Delete the individual shaders once we are done
  glDeleteShader(myVertexShader);
  glDeleteShader(myFragmentShader);

  return programObject;
}
