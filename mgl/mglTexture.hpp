////////////////////////////////////////////////////////////////////////////////
//
// Textures (small example for 2D textures)
//
// Copyright (c)2022-23 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_TEXTURE_HPP
#define MGL_TEXTURE_HPP

#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mglSampler.hpp"
#include "mglShader.hpp"
#include "perlinNoise.hpp"

namespace mgl {

class Texture;
class Texture2D;
class Texture3D;
struct TextureInfo;

//////////////////////////////////////////////////////////////////////// TEXTURE

class Texture {
protected:
  GLuint id;

public:
  Texture();
  ~Texture();
  virtual void bind() = 0;
  virtual void unbind() = 0;
  GLint getId();
};

//////////////////////////////////////////////////////////////////// TextureInfo

struct TextureInfo {
  GLenum unit;                // GL_TEXTUREi
  GLuint index;               // sampler index in shader
  std::string uniform;        // uniform name in shader
  Texture *texture = nullptr; // Texture (engine object)
  Sampler *sampler = nullptr; // Sampler (engine object)

  TextureInfo(GLenum textureunit, GLuint index, const std::string &uniform,
              Texture *texture, Sampler *sampler);
  void updateShader(ShaderProgram *shader);
};

/////////////////////////////////////////////////////////////////////// TEXTURES

class Texture2D : public Texture {
public:
  void bind() override;
  void unbind() override;
  void load(const std::string &filename);
  void generatePerlinNoiseTexture(const unsigned int height, const unsigned int width);
};

class Texture3D : public Texture {
public:
    enum Type { WOOD, MARBLE };

    void bind() override;
    void unbind() override;
    //void load(const std::string& filename);
    void generatePerlinNoiseTexture(const unsigned int height, const unsigned int width, const unsigned int depth);
    void generatePerlinNoiseTexture2(const unsigned int height, const unsigned int width, const unsigned int depth);
    void generatePerlinNoiseTexture3(unsigned int width, unsigned int height, unsigned int depth, Type type);
    void generatePerlingNoiseTextureSubLevel(PerlinNoiseGenerator* perlinNoise, unsigned int level);
    void generateWoodSublevel(PerlinNoiseGenerator* perlinNoise, unsigned int width, unsigned int height, unsigned int depth, unsigned int level);
    void generateMarbleSublevel(PerlinNoiseGenerator* perlinNoise, unsigned int width, unsigned int height, unsigned int depth, unsigned int level);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace mgl

#endif /* MGL_TEXTURE_HPP */
