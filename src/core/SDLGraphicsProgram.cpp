#include "core/SDLGraphicsProgram.hpp"

#include "core/Error.hpp"
#include "core/MeshGenerator.hpp"
#include "core/Ray.hpp"

#include "physics/SoftbodyObject.hpp"

#include "rendering/Renderer.hpp"
#include "rendering/Window.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <iostream>

using MeshType = MeshGenerator::MeshType;

Entity *currentEntity;

SDLGraphicsProgram::SDLGraphicsProgram(Window *window, Renderer *renderer)
    : _window(window), _renderer(renderer) {
  _grabber.setCamera(&_renderer->getCamera());
};

void SDLGraphicsProgram::input(float deltaTime) {
  int mouseY = _window->getHeight() / 2;
  int mouseX = _window->getWidth() / 2;
  float moveSpeed = 20.0f * deltaTime;
  Camera &camera = _renderer->getCamera();

  // Event handler that handles various events in SDL
  // that are related to input and output
  SDL_Event e;
  // Handle events on queue
  while (SDL_PollEvent(&e) != 0) {
    switch (e.type) {
    case SDL_QUIT:
      std::cout << "Goodbye!" << std::endl;
      _quit = true;
      break;
    case SDL_KEYDOWN:
      if (e.key.keysym.sym == SDLK_ESCAPE) {
        std::cout << "ESC: Goodbye!" << std::endl;
        _quit = true;
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      if (e.button.button == SDL_BUTTON_LEFT ||
          e.button.button == SDL_BUTTON_RIGHT) {
        int x = e.motion.x;
        int y = _window->getHeight() - e.motion.y;
        // unproject to build a ray from the camera to the mouse position
        glm::vec3 rayStart = glm::unProject(
            glm::vec3(x, y, 0.0f), camera.getViewMatrix(),
            camera.getProjectionMatrix(),
            glm::vec4(0, 0, _window->getWidth(), _window->getHeight()));
        glm::vec3 rayEnd = glm::unProject(
            glm::vec3(x, y, 1.0f), camera.getViewMatrix(),
            camera.getProjectionMatrix(),
            glm::vec4(0, 0, _window->getWidth(), _window->getHeight()));
        glm::vec3 rayDir = glm::normalize(rayEnd - rayStart);
        glm::vec3 invDir = 1.0f / rayDir;

        Ray ray{camera.getTransform().getPosition(), rayDir, invDir};

        if (e.button.button == SDL_BUTTON_LEFT) {
          if (_grabber.isGrabbing()) {
            break;
          }
          // Try to grab an object
          _grabber.grab(ray, _rootNode);
        } else {
          // Try to delete an object
          _grabber.deleteObject(ray, _rootNode);
        }
      }
      break;
    case SDL_MOUSEBUTTONUP:
      if (e.button.button == SDL_BUTTON_LEFT) {
        _grabber.release();
      }
      break;
    case SDL_MOUSEMOTION:
      // Capture the change in the mouse position
      mouseX = e.motion.x;
      mouseY = e.motion.y;
      if (_freeLook) {
        camera.mouseLook(mouseX, mouseY);
      }

      if (_grabber.isGrabbing()) {
        int x = e.motion.x;
        int y = _window->getHeight() - e.motion.y;
        // unproject to build a ray from the camera to the mouse position
        glm::vec3 rayStart = glm::unProject(
            glm::vec3(x, y, 0.0f), glm::mat4(1.0f),
            camera.getProjectionMatrix(),
            glm::vec4(0, 0, _window->getWidth(), _window->getHeight()));
        glm::vec3 rayEnd = glm::unProject(
            glm::vec3(x, y, 1.0f), glm::mat4(1.0f),
            camera.getProjectionMatrix(),
            glm::vec4(0, 0, _window->getWidth(), _window->getHeight()));
        glm::vec3 rayDir = glm::normalize(rayEnd - rayStart);
        _grabber.moveGrabbed(rayDir);
      }
      break;
    default:
      break;
    }
  }

  // Retrieve keyboard state
  const Uint8 *state = SDL_GetKeyboardState(NULL);

  // Camera
  // Update our position of the camera
  if (state[SDL_SCANCODE_W]) {
    camera.moveForward(moveSpeed);
  }
  if (state[SDL_SCANCODE_S]) {
    camera.moveBackward(moveSpeed);
  }
  if (state[SDL_SCANCODE_A]) {
    camera.moveLeft(moveSpeed);
  }
  if (state[SDL_SCANCODE_D]) {
    camera.moveRight(moveSpeed);
  }
  if (state[SDL_SCANCODE_SPACE]) {
    camera.moveUp(moveSpeed);
  }
  if (state[SDL_SCANCODE_Q]) {
    camera.moveDown(moveSpeed);
  }

  // Toggle free look
  if (state[SDL_SCANCODE_TAB]) {
    SDL_Delay(200);
    _freeLook = !_freeLook;
  }

  // Spawn objects
  if (state[SDL_SCANCODE_1] || state[SDL_SCANCODE_2] || state[SDL_SCANCODE_3] ||
      state[SDL_SCANCODE_4]) {
    SDL_Delay(150);
    MeshType type;
    if (state[SDL_SCANCODE_1]) {
      type = MeshType::CUBE;
    } else if (state[SDL_SCANCODE_2]) {
      type = MeshType::ICOSAHEDRON;
    } else if (state[SDL_SCANCODE_3]) {
      type = MeshType::BUNNY_REDUCED;
    } else {
      type = MeshType::BUNNY;
    }
    currentEntity = addObject(type);
  }

  // Toggle polygon mode
  if (state[SDL_SCANCODE_Z]) {
    SDL_Delay(200);
    _renderer->flipPolygonMode();
  }

  // Toggle depth debug mode
  if (state[SDL_SCANCODE_X]) {
    SDL_Delay(200);
    _debug = !_debug;
  }

  _lastTime = SDL_GetTicks();
}

void SDLGraphicsProgram::update(float deltaTime) {
  _rootNode.update(deltaTime);
}

void SDLGraphicsProgram::render() const {
  _renderer->render(_rootNode);
  if (_debug) {
    _renderer->renderDebugQuad();
  }
}

void SDLGraphicsProgram::run() {
  SDL_WarpMouseInWindow(_window->getWindow(), _window->getWidth() / 2,
                        _window->getHeight() / 2);
  // Disable for grabbing
  // SDL_SetRelativeMouseMode(SDL_TRUE);

  _lastTime = SDL_GetTicks();

  while (!_quit) {
    Uint32 currentTime = SDL_GetTicks();
    Uint32 delta = std::max((unsigned int)1, currentTime - _lastTime);
    _lastTime = currentTime;

    glCheckError("run", 126);

    float deltaTime = delta / 1000.0f;
    input(deltaTime);
    update(deltaTime);
    render();

    glCheckError("run", 132);

    // if (delta < 16) {
    //   std::cout << "Delaying: " << 16 - delta << "ms" << std::endl;
    //   SDL_Delay(16 - delta);
    // }
    // std::cout << "delta: " << delta << "ms\n";

    _window->swapBuffers();
  }
}

Entity *SDLGraphicsProgram::addObject(MeshType type) {
  Entity *entity = nullptr;
  switch (type) {
  case MeshType::CUBE:
    entity = addObject(MeshGenerator::generateCube());
    break;
  case MeshType::ICOSAHEDRON:
    entity = addObject(MeshGenerator::generateIcosahedron());
    break;
  case MeshType::BUNNY:
    entity = addObject(BUNNY_PATH);
    break;
  case MeshType::BUNNY_REDUCED:
    entity = addObject(BUNNY_REDUCED_PATH);
    break;
  default:
    break;
  }
  entity->getTransform().setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
  entity->getObject()->setColor(glm::vec3(1.0f, 0.65f, 0.0f));
  return entity;
}

void SDLGraphicsProgram::getOpenGLVersionInfo() {
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << "\n";
}

void SDLGraphicsProgram::init() {
  // Add a cube as the ground
  Entity *ground = addObject(MeshGenerator::generateCube());
  ground->getTransform().setScale(glm::vec3(100.0f, 1.0f, 100.0f));
  ground->getTransform().setPosition(glm::vec3(0.0f, -0.5f, 0.0f));
  ground->getObject()->setStatic(true);

  // Walls
  Entity *wall1 = addObject(MeshGenerator::generateCube());
  wall1->getTransform().setPosition(glm::vec3(0.0f, 0.0f, -10.0f));
  wall1->getTransform().setScale(glm::vec3(21.0f, 5.0f, 1.0f));
  wall1->getObject()->setStatic(true);
  Entity *wall2 = addObject(MeshGenerator::generateCube());
  wall2->getTransform().setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
  wall2->getTransform().setScale(glm::vec3(21.0f, 5.0f, 1.0f));
  wall2->getObject()->setStatic(true);
  Entity *wall3 = addObject(MeshGenerator::generateCube());
  wall3->getTransform().setPosition(glm::vec3(-10.0f, 0.0f, 0.0f));
  wall3->getTransform().setScale(glm::vec3(19.0f, 5.0f, 1.0f));
  wall3->getTransform().rotate(0.0f, 90.0f, 0.0f);
  wall3->getObject()->setStatic(true);
  Entity *wall4 = addObject(MeshGenerator::generateCube());
  wall4->getTransform().setPosition(glm::vec3(10.0f, 0.0f, 0.0f));
  wall4->getTransform().setScale(glm::vec3(19.0f, 5.0f, 1.0f));
  wall4->getTransform().rotate(0.0f, 90.0f, 0.0f);
  wall4->getObject()->setStatic(true);

  // Add some solids
  Entity *cube =
      addObject(MeshGenerator::generateCube(), glm::vec3(1.0f, 0.65f, 0.0f));
  cube->getTransform().setPosition(glm::vec3(-3.0f, 3.0f, 0.0f));

  _renderer->getLight().getTransform().setPosition(
      glm::vec3(15.0f, 15.0f, 15.0f));
  _renderer->getLight().update(0.0f);

  _renderer->getCamera().getTransform().setPosition(
      glm::vec3(-10.0f, 12.0f, 10.0f));
  _renderer->getCamera().lookAt(glm::vec3(0.0f));
}
