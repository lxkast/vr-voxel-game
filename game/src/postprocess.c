#include <logging.h>
#include "postprocess.h"
#include "shaderutil.h"
#include "vertices.h"

#define DISTORTION_STRENTH 0.03f
#define CENTER_X 0.52f // where 1 is the middle of the screen, and 0 is the side
#define CENTER_Y 0.5f // where
#define SCALE 1.0f

/*
    sets up the quad VAO and VBO
*/
static void postProcess_initVertices(postProcess_t *postProcess) {
    glGenVertexArrays(1, &postProcess->vao);
    glGenBuffers(1, &postProcess->vbo);
    glBindVertexArray(postProcess->vao);
    glBindBuffer(GL_ARRAY_BUFFER, postProcess->vbo);
    glBufferData(GL_ARRAY_BUFFER, quadVerticesSize, quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
}

/*
    creates a frame buffer object, binding a texture colour buffer and a render buffer to it
*/
static void postProcess_initFramebuffer(postProcess_buffer_t *renderbuffer, int width, int height) {
    glGenFramebuffers(1, &renderbuffer->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, renderbuffer->framebuffer);

    glGenTextures(1, &renderbuffer->textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, renderbuffer->textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderbuffer->textureColorbuffer, 0);

    glGenRenderbuffers(1, &renderbuffer->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer->rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("Failed to create framebuffer: %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
    Initialises everything needed to enable post-processing (shader, frame buffer etc.)
*/
void postProcess_init(postProcess_t *postProcess, GLuint shaderProgram, int width, int height) {
    postProcess->program = shaderProgram;
    postProcess->buffer_width = width / 2;
    postProcess->buffer_height = height;
    postProcess_initFramebuffer(&postProcess->leftFramebuffer, width / 2, height);
    postProcess_initFramebuffer(&postProcess->rightFramebuffer, width / 2, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    postProcess_initVertices(postProcess);
}

/*
   Uses the two frame buffers to apply the post-processing shader to them
*/
void postProcess_draw(postProcess_t *postProcess) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glUseProgram(postProcess->program);
    glBindVertexArray(postProcess->vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->leftFramebuffer.textureColorbuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, postProcess->rightFramebuffer.textureColorbuffer);
    glUniform1i(glGetUniformLocation(postProcess->program, "leftTexture"), 0);
    glUniform1i(glGetUniformLocation(postProcess->program, "rightTexture"), 1);
    glUniform1f(glGetUniformLocation(postProcess->program, "distortionStrength"), DISTORTION_STRENTH);
    glUniform1f(glGetUniformLocation(postProcess->program, "centerX"), CENTER_X);
    glUniform1f(glGetUniformLocation(postProcess->program, "centerY"), CENTER_Y);
    glUniform1f(glGetUniformLocation(postProcess->program, "scale"), SCALE);

    glDrawArrays(GL_TRIANGLES, 0, 12);
}

/*
    All future draw calls will draw to the specified buffer

    Can be undone using glBindFramebuffer(GL_FRAMEBUFFER, 0)
*/
void postProcess_bindBuffer(postProcess_buffer_t *buffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, buffer->framebuffer);
}
