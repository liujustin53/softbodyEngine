#pragma once

#include <string>

class Shader;

class Texture {
public:
  enum TextureType { DIFFUSE, SPECULAR, NORMAL };

  Texture(const std::string &path, TextureType type);
  ~Texture();

  void load();
  void bind(const Shader &shader, unsigned int slot = 0) const;
  void unbind() const;

private:
  unsigned int _id;
  TextureType _type;
  std::string _path;
};
