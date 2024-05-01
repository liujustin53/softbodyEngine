#include "rendering/Renderer.hpp"

#include "core/Entity.hpp"

#include "rendering/Window.hpp"

Renderer::Renderer(const Window &window)
    : _camera(window.getWidth(), window.getHeight()),
      _shader("res/shaders/shadow_vert.glsl", "res/shaders/shadow_frag.glsl"),
      _lightShader("res/shaders/light_vert.glsl",
                   "res/shaders/light_frag.glsl"),
      _depthShader("res/shaders/shadow_depth_vert.glsl",
                   "res/shaders/shadow_depth_frag.glsl"),
      _debugDepthShader("res/shaders/debug_vert.glsl",
                        "res/shaders/debug_frag.glsl"),
      _window(&window) {}

void Renderer::renderDebugQuad() const { _depthMap.renderDebugQuad(); }

void Renderer::render(const Entity &rootNode) const {
  // Enable depth test and face culling (to fix shadow peter panning)
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // Set the polygon fill mode
  glPolygonMode(GL_FRONT_AND_BACK, _polygonMode);

  // Initialize clear color
  // This is the background of the screen.
  glClearColor(0.1f, 4.f, 7.f, 1.f);

  // Clear color buffer and Depth Buffer
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  // Render depth of scene
  _depthShader.use();
  _depthShader.setMat4("u_LightSpaceMatrix", _light.lightSpaceMatrix);

  _depthMap.bind();
  rootNode.draw(_depthShader);
  _depthMap.unbind();

  // Reset viewport
  glViewport(0, 0, _window->getWidth(), _window->getHeight());
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  // Render scene
  _shader.use();
  _shader.setMat4("u_View", _camera.getViewMatrix());
  _shader.setMat4("u_Projection", _camera.getProjectionMatrix());
  _shader.setMat4("u_LightSpaceMatrix", _light.lightSpaceMatrix);
  _shader.setVec3("u_ViewPos", _camera.getTransform().getPosition());
  _shader.setFloat("u_Material.shininess", 32.0f);

  // Pass in light properties
  _shader.setLight("u_PointLight", _light);

  // Set the shadow map
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _depthMap.depthMap);
  _shader.setInt("u_DepthMap", 0);

  rootNode.draw(_shader);

  // Render light
  _lightShader.use();
  _lightShader.setMat4("u_View", _camera.getViewMatrix());
  _lightShader.setMat4("u_Projection", _camera.getProjectionMatrix());
  _light.draw(_lightShader);

  // Render debug depth map
  _debugDepthShader.use();
  _debugDepthShader.setInt("u_DepthMap", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _depthMap.depthMap);
  // renderQuad();
}

void Renderer::flipPolygonMode() {
  _polygonMode = _polygonMode == GL_FILL ? GL_LINE : GL_FILL;
}
