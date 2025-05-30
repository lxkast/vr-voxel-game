#pragma once
#include <glad/gl.h>

typedef struct {
    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rbo;
} postProcess_Renderbuffer_t;

typedef struct {
    GLuint vao;
    GLuint vbo;
    postProcess_Renderbuffer_t leftFramebuffer;
    postProcess_Renderbuffer_t rightFramebuffer;
    GLuint program;
} postProcess_t;

extern void postProcess_init(postProcess_t *postProcess, GLuint shaderProgram, int width, int height);

extern void postProcess_draw(postProcess_t *postProcess);

extern void postProcess_bindBuffer(postProcess_Renderbuffer_t *renderbuffer);