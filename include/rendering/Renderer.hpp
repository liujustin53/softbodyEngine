#pragma once

#include "core/Camera.hpp"

#include "DepthMap.hpp"
#include "Light.hpp"
#include "Shader.hpp"

class Window;
class Entity;

class Renderer {
public:
  Renderer(const Window &window);

  void render(const Entity &rootNode) const;
  void renderDebugQuad() const;

  void flipPolygonMode();

  Camera &getCamera() { return _camera; }
  Light &getLight() { return _light; }

private:
  Camera _camera;

  Shader _shader;
  Shader _lightShader;
  Shader _depthShader;
  DepthMap _depthMap;

  Shader _debugDepthShader;

  // std::vector<Light> _lights;
  // unsigned int _activeLight = 0;
  Light _light;

  const Window *_window;
  GLenum _polygonMode = GL_FILL;
};
