#pragma once
#include <glad/gl.h>

/*
    returns a handle to a texture object, given a path and wrapping/filter options

    usage:
    GLuint texture = loadTextureRGBA("textures/dirt.jpg", GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
*/
GLuint loadTextureRGBA(const char* path, GLint wrapS, GLint wrapT, GLint minFilter, GLint magFilter);