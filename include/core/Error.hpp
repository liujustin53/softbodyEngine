#pragma once

#include <glad/glad.h>
#include <iostream>

// This assert inserts a breakpoint in your code!
// That way we can get the line number and file.
// #define ASSERT(x) if(!(x)) __debugbreak(); (__debugbreak() is only available
// on MSVC)
#define glCall(x)                                                              \
  glClearErrorStates();                                                        \
  x;                                                                           \
  glCheckError(#x, __LINE__);

// New error handling routine
static void glClearErrorStates() {
  // Return all of the errors that occur.
  while (glGetError() != GL_NO_ERROR) {
    ;
  }
}

// Returns false if an error occurs
static bool glCheckError(const char *function, int line) {
  while (GLenum error = glGetError()) {
    // __LINE__ is a special preprocessor macro that
    // Can tell us what line any errors occurred on.
    std::cout << "[OpenGL Error]" << error << "|" << function
              << " Line: " << line << "\n";
    return false;
  }
  return true;
}
