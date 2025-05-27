#include <logging.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture.h"

GLuint loadTextureRGB(const char* path, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (!data) {
        LOG_FATAL("Failed to load texture at path: %s", path);
    }
    if (nrChannels != 3) {
        LOG_FATAL("Wrong number of channels when loading RGB texture: %s", path);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    return textureID;
}