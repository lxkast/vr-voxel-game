#include <logging.h>
#include "stb_image.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION

GLuint loadTexture(const char *path,
                   const GLint wrapS,
                   const GLint wrapT,
                   const GLint minFilter,
                   const GLint magFilter) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (!data) {
        LOG_FATAL("Failed to load texture at path: %s", path);
    }
    GLint format;
    switch (nrChannels) {
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default:
            LOG_FATAL("Invalid number of channels");
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    return textureID;
}