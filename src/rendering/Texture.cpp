#include "rendering/Texture.hpp"

#include "rendering/PPM.hpp"
#include "rendering/Shader.hpp"

#include <glad/glad.h>

Texture::Texture(const std::string &path, TextureType type)
    : _path(path), _type(type) {}

Texture::~Texture() { glDeleteTextures(1, &_id); }

void Texture::load() {
  glGenTextures(1, &_id);
  glBindTexture(GL_TEXTURE_2D, _id);

  // set the texture wrapping/filtering options (on the currently bound
  // texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // load and generate the texture
  PPM image(_path);
  image.flipVertical();
  image.flipHorizontal();
  int width = image.getWidth();
  int height = image.getHeight();
  int numChannels = 3;
  uint8_t *data = image.pixelDataPtr();

  GLenum format;
  if (numChannels == 1) {
    format = GL_RED;
  } else if (numChannels == 3) {
    format = GL_RGB;
  } else if (numChannels == 4) {
    format = GL_RGBA;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind(const Shader &shader, unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  std::string name = "u_Material.";
  if (_type == TextureType::DIFFUSE) {
    name += "diffuse";
  } else if (_type == TextureType::SPECULAR) {
    name += "specular";
  } else if (_type == TextureType::NORMAL) {
    name += "normal";
  }
  shader.setInt(name, slot);
  glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
