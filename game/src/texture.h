#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/gl.h>

/**
 * @brief Returns a handle to a texture object, given a path and wrapping/filter options
 * @param path The filepath to the texture
 * @param wrapS The wrapS
 * @param wrapT The wrapT
 * @param minFilter The min filter
 * @param magFilter The mag filter
 * @return The handle to the texture object
 */
GLuint loadTexture(const char* path, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter);

#endif
