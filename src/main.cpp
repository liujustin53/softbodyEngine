#include <iostream>

#include "core/Error.hpp"
#include "core/SDLGraphicsProgram.hpp"

#include "rendering/Renderer.hpp"
#include "rendering/Window.hpp"

int main(int argc, char *args[]) {
  std::cout << "Player controls:\n"
            << "  WASD - Move\n"
            << "  Space - Move up\n"
            << "  Q - Move down\n"
            << "  Mouse - Look around\n"
            << "  Tab - Toggle Mouse Look\n"
            << "  Hold Left click - Grab object\n"
            << "  Right click - Delete object\n"
            << "Object spawn controls:\n"
            << "  1 - Spawn cube\n"
            << "  2 - Spawn icosahedron\n"
            << "  3 - Spawn reduced-faces bunny\n"
            << "  4 - Spawn full mesh bunny (quite laggy)\n"
            << "Debug controls:\n"
            << "  Z - Toggle wireframe\n"
            << "  X - Toggle depth map FBO\n"
            << "Escape - Quit\n";

  Window window(1600, 900);
  glCheckError("Window", 17);
  Renderer renderer(window);
  glCheckError("Renderer", 19);
  SDLGraphicsProgram graphicsProgram(&window, &renderer);
  glCheckError("SDLGraphicsProgram", 21);

  graphicsProgram.init();
  graphicsProgram.run();

  return 0;
}
