#pragma once
#include <glad/gl.h>

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint rbo;
    GLuint textureColorbuffer;
    GLuint framebuffer;
    GLuint program;
} postProcess_t;

extern void postProcess_init(postProcess_t *postProcess, GLuint shaderProgram, int width, int height);

extern void postProcess_draw(postProcess_t *postProcess);