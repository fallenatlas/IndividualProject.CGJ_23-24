#include <cassert>
#include <sstream>

#include "mglTexture.hpp"
#include "perlinNoise.hpp"
#include "stb_image.h"

namespace mgl {

//////////////////////////////////////////////////////////////////// TextureInfo

TextureInfo::TextureInfo(GLenum _unit, GLuint _index,
                         const std::string &_uniform, Texture *_texture,
                         Sampler *_sampler) {
  unit = _unit;
  uniform = _uniform;
  index = _index;
  texture = _texture;
  sampler = _sampler;
}

void TextureInfo::updateShader(ShaderProgram *shader) {
  glActiveTexture(unit);
  texture->bind();
  if (sampler)
    sampler->bind(index);
  glUniform1i(shader->Uniforms[uniform].index, index);
}

//////////////////////////////////////////////////////////////////////// Texture

Texture::Texture() : id(-1) {}

Texture::~Texture() {}

GLint Texture::getId() {
    return id;
}

////////////////////////////////////////////////////////////////////// Texture2D

void Texture2D::bind() { glBindTexture(GL_TEXTURE_2D, id); }

void Texture2D::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture2D::load(const std::string &filename) {

  stbi_set_flip_vertically_on_load(true);
  int width, height, channels;
  std::cout << "Loading image file " << filename << "... ";
  unsigned char *image =
      stbi_load(filename.c_str(), &width, &height, &channels, 0);
  assert(channels == 4);
  if (image == nullptr) {
    std::cout << "error." << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "ok. " << std::endl;
  }

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  // Pré OpenGL v3.30 (still compatible with core)

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
  //                GL_LINEAR_MIPMAP_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image);
  // syntax: glTexImage2D(target, level, internalformat, width, height, border,
  // format, type, data)

  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image);
}

double harmonicNoise(PerlinNoiseGenerator* perlinNoise, int numberOctaves, float b, float a, double x, double y) {
    double endNoise = 0;
    float totalAmplitude = 0.0;

    for (int i = 0; i < numberOctaves; ++i) {
        float amplitude = glm::pow((float)a, (float)i);// 1 / 2 * i;
        totalAmplitude += amplitude;
    }

    for (int i = 0; i < numberOctaves; ++i) {
        float frequency = glm::pow((float)b, (float)i); //2 * i;
        float amplitude = glm::pow((float)a, (float)i);// 1 / 2 * i; //0.7

        endNoise += perlinNoise->noise(frequency * x, frequency * y, 0.45) * amplitude / totalAmplitude;
    }

    return endNoise;
}



void Texture2D::generatePerlinNoiseTexture(const unsigned int height, const unsigned int width) {

    //GLubyte checkImage[height][width][1];
    unsigned char** noise = new unsigned char*[height];
    for (unsigned int i = 0; i < height; i++) {
        noise[i] = new unsigned char[width];
    }

    GLubyte image[512][512] = { 0 };

    int persistence, amplitude;

    //static GLuint texName;
    PerlinNoiseGenerator* perlinNoise = new PerlinNoiseGenerator();

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double x = (double)j / ((double)width);
            double y = (double)i / ((double)height);

            //double n = perlinNoise->noise(10 * x, 10 * y, 0.8);

            /*
            double n = harmonicNoise(perlinNoise, 8, 2, 0.6, 2 * x, 2 * y); // 0.7
            double normalizedN = (n + 1.0) / 2.0;

            // marble
            double xPeriod = 0.5; //defines repetition of marble lines in x direction 0.0
            double yPeriod = 1.0; //defines repetition of marble lines in y direction 10.0
            //turbPower = 0 ==> it becomes a normal sine pattern
            double turbPower = 5.0; //makes twists 4.0
            double turbSize = 32.0; //initial size of the turbulence

            double xyValue = j * xPeriod / width + i * yPeriod / height + turbPower * normalizedN; // x and y
            double sineValue = fabs(sin(xyValue * 3.14159));

            if (sineValue >= -0.001 && sineValue < 0.1) sineValue += 0.05 * (1.0 - sineValue);
            image[i][j] = (GLubyte)floor((sineValue > 0.0 && sineValue < 1.0 ? sineValue + sineValue * (1.0-sineValue) : sineValue) * 256);
            */

            //double cosValue = 255 * cos(i + n) * 0.5 + 0.5;
            //double cosValue = 255 * cos(2 * x + normalizedN) * 0.5 + 0.5;

            // wood
            
            double strechedNoise = harmonicNoise(perlinNoise, 8, 2, 0.7, 8 * x, 1 * y);
            double normalizedSN = (strechedNoise + 1.0) / 2.0;
            
            double xPeriod = 3.0; //defines repetition of marble lines in x direction
            double yPeriod = 0.0; //defines repetition of marble lines in y direction
            //turbPower = 0 ==> it becomes a normal sine pattern
            double turbPower = 1.0; //makes twists
            double turbSize = 32.0; //initial size of the turbulence

            double n = harmonicNoise(perlinNoise, 4, 2, 0.45, 0.5 * x, 0.5 * y);
            double normalizedN = (n + 1.0) / 2.0;
            double xyValue = j * xPeriod / width + i * yPeriod / height + turbPower * normalizedN;
            double zebraNoise = 256 * fabs(sin(xyValue * 3.14159));

            //n = perlinNoise->noise(x, y, 0.8);
            //normalizedN = (n + 1.0) / 2.0;
            n = 20 * normalizedN;
            n = n - floor(n);
            //n = turbPower * n;

            //double fineGrain = perlinNoise->noise(128 * x, 32 * y, 0.8);
            double fineGrain = 2 * harmonicNoise(perlinNoise, 10, 2, 0.9, 4 * x, 1 * y);
            double normalizedFN = (fineGrain + 1.0) / 2.0;

            double fineGrain3 = perlinNoise->noise(80 * x, 10 * y, 0.9);
            double normalizedFN3 = (fineGrain3 + 1.0) / 2.0;
           
            double fineGrain2 = perlinNoise->noise(256 * x, 256 * y, 0.8);
            double normalizedFN2 = (fineGrain2 + 1.0) / 2.0;

            //noise[i][j] = (unsigned char) floor(n * 255);

            image[i][j] = (GLubyte)floor((1-(normalizedFN * normalizedFN3) * normalizedFN2) * 256);
            double thing = (1 - (normalizedFN * normalizedFN3)) * normalizedFN2;
            double thing2 = (1 - (normalizedFN * normalizedFN3));
            thing = thing > 0.2 ? 1.0 : thing;
            thing2 = thing2 < 0.86 ? thing2 - (0.86 - thing2) * (0.0 - thing2) : thing2;
            thing2 = thing2 < 0.86 ? 0.0 : thing2 - 1.0 * (1-thing2);
            //(sineValue > 0.0 && sineValue < 1.0 ? sineValue + sineValue * (1.0 - sineValue) : sineValue
            image[i][j] = (GLubyte)floor((1-thing2) * 255);
            image[i][j] = (GLubyte) floor((n * normalizedFN * (1 - thing2)) * 256);
            
            //image[i][j][1] = (unsigned char)floor(n * 255);
            //image[i][j][2] = (unsigned char)floor(n * 255);

            //std::cout << "value: " << floor(n * 255) << std::endl;
        }
    }

    /*
    for (unsigned int i = 0; i < height; ++i) {     // y
        for (unsigned int j = 0; j < width; ++j) {  // x
            double x = (double)j / ((double)width);
            double y = (double)i / ((double)height);

            // Typical Perlin noise
            double n = perlinNoise->noise(10 * x, 10 * y, 0.8);

            // Wood like structure
            //n = 20 * pn.noise(x, y, 0.8);
            //n = n - floor(n);

            // Map the values to the [0, 255] interval, for simplicity we use 
            // tones of grey
            noise[i][j] = floor(255 * n);
        }
    }
    */

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // Pré OpenGL v3.30 (still compatible with core)

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    //                GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
        GL_UNSIGNED_BYTE, image);
    // syntax: glTexImage2D(target, level, internalformat, width, height, border,
    // format, type, data)

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    //delete[] noise;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace mgl
