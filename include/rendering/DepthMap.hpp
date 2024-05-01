#pragma once

class DepthMap {
public:
  unsigned int depthMap;

  DepthMap(int width = 2048, int height = 2048);
  ~DepthMap();

  void bind() const;
  void unbind() const;

  void renderDebugQuad() const;

private:
  unsigned int _fbo;
  int _width;
  int _height;

  // For debugging
  unsigned int _vao, _vbo;
};
