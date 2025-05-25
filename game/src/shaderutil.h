#pragma once

#include <glad/gl.h>

typedef enum {
    SU_VERTEX,
    SU_FRAGMENT,
    SU_DETERMINE
} su_shader_t;

int su_initialiseShader(GLuint *shader, const char *fileName, su_shader_t type);

int su_createShaderProgramFromHandles(GLuint *program, int n, const GLuint *shaderHandles);

int su_createShaderProgramFromFilenames(GLuint *program, int n, ...);
