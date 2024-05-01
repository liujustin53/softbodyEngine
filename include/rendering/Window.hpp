#pragma once

#include <string>

#include <SDL2/SDL.h>

class Window {
public:
  Window(int width = 1280, int height = 720,
         const std::string &title = "Graphics");
  ~Window();

  int getWidth() const { return _width; }
  int getHeight() const { return _height; }
  SDL_Window *getWindow() { return _window; }

  void swapBuffers() { SDL_GL_SwapWindow(_window); }

private:
  int _width;
  int _height;
  std::string _title;

  SDL_Window *_window;
  SDL_GLContext _context;
};
