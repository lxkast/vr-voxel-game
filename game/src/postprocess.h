#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <glad/gl.h>

typedef struct {
    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rbo;
} postProcess_buffer_t;

typedef struct {
    GLuint vao;
    GLuint vbo;
    postProcess_buffer_t leftFramebuffer;
    postProcess_buffer_t rightFramebuffer;
    int buffer_width;
    int buffer_height;
    GLuint program;
} postProcess_t;

/*
    Initialises everything needed to enable post-processing (shader, frame buffer etc.)
*/
extern void postProcess_init(postProcess_t *postProcess, GLuint shaderProgram, int width, int height);

/*
   Uses the two frame buffers to apply the post-processing shader to them
*/
extern void postProcess_draw(postProcess_t *postProcess);

/*
    All future draw calls will draw to the specified buffer

    Can be undone using glBindFramebuffer(GL_FRAMEBUFFER, 0)
*/
extern void postProcess_bindBuffer(postProcess_buffer_t *buffer);

#endif
