#pragma once

#include <SDL2/SDL.h>

#include "Entity.hpp"
#include "MeshGenerator.hpp"
#include "physics/Grabber.hpp"

class Window;
class Renderer;

class SDLGraphicsProgram {
public:
  SDLGraphicsProgram(Window *window, Renderer *renderer);

  void run();

  /**
   * @brief Add an object to the scene graph of a specific type.
   *
   * @tparam T Type of object to add
   * @tparam Args
   * @param args Arguments to pass to the object constructor
   * @return Entity*
   */
  template <typename... Args> Entity *addObject(Args &&...args) {
    return _rootNode.addChild(std::forward<Args>(args)...);
  }

  // Spawns an object of the specified type
  Entity *addObject(MeshGenerator::MeshType type);

  void getOpenGLVersionInfo();

  // initializing a test scene
  void init();

private:
  bool _quit = false;
  bool _debug = false;
  bool _freeLook = true;

  // Stored to prevent a large delta time after delays
  Uint32 _lastTime;

  // Scene graph and objects
  Entity _rootNode;

  // Grabber
  Grabber _grabber;

  Window *_window;
  Renderer *_renderer;

  inline static const std::string BUNNY_PATH =
      "res/objects/bunny/bunny_centered_fixed.obj";
  inline static const std::string BUNNY_REDUCED_PATH =
      "res/objects/bunny/bunny_centered_reduced_fixed.obj";

  void input(float deltaTime);
  void update(float deltaTime);
  void render() const;
};
