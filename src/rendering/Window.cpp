#include "rendering/Window.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>

Window::Window(int width, int height, const std::string &title)
    : _width(width), _height(height), _title(title) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
    exit(1);
  }

  // Setup the OpenGL Context
  // Use OpenGL 4.1 core or greater
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  // We want to request a double buffer for smooth updating.
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  _window = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, _width, _height,
                             SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (!_window) {
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    exit(1);
  }

  _context = SDL_GL_CreateContext(_window);
  if (!_context) {
    std::cerr << "Failed to create OpenGL context: " << SDL_GetError()
              << std::endl;
    exit(1);
  }

  // Initialize GLAD Library
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    exit(1);
  }
}

Window::~Window() {
  SDL_GL_DeleteContext(_context);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}
