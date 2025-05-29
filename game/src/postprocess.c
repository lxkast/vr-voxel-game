#include <logging.h>
#include "postprocess.h"
#include "shaderutil.h"
#include "vertices.h"

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

void postProcess_init(postProcess_t *postProcess, GLuint shaderProgram, int width, int height) {
    postProcess->program = shaderProgram;
    glGenFramebuffers(1, &postProcess->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, postProcess->framebuffer);

    glGenTextures(1, &postProcess->textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, postProcess->textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcess->textureColorbuffer, 0);

    glGenRenderbuffers(1, &postProcess->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, postProcess->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, postProcess->rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("Failed to create framebuffer: %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    postProcess_initVertices(postProcess);
}

void postProcess_draw(postProcess_t *postProcess) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(postProcess->program);
    glBindVertexArray(postProcess->vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcess->textureColorbuffer);
    glUniform1i(glGetUniformLocation(postProcess->program, "screenTexture"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}