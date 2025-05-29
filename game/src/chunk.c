#include "chunk.h"
#include <cglm/cglm.h>
#include <string.h>
#include "vertices.h"

static void chunk_createMesh(chunk_t *c) {
    static const size_t bytesPerBlock = sizeof(float) * 36 * 5;

    float *buf = (float *)malloc(CHUNK_SIZE_CUBED * bytesPerBlock);

    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                float *blockPtr = buf + bytesPerBlock * (16 * 16 * i + 16 * j + k);
                memcpy(blockPtr, grassVertices, bytesPerBlock);
                for (int l = 0; l < 36; l++) {
                    blockPtr[5 * l] += 0.5f + i;
                    blockPtr[5 * l + 1] += 0.5f + j;
                    blockPtr[5 * l + 2] += 0.5f + k;
                }
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    glBufferData(GL_ARRAY_BUFFER, CHUNK_SIZE_CUBED * bytesPerBlock, buf, GL_STATIC_DRAW);

    free(buf);
}

void chunk_create(chunk_t *c, const int cx, const int cy, const int cz, const block_t block) {
    c->cx = cx;
    c->cy = cy;
    c->cz = cz;

    int *ptr = c->blocks;
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
        ptr[i] = block;
    }

    glGenBuffers(1, &c->vbo);
    chunk_createMesh(c);
}


void chunk_draw(const chunk_t *c, const int modelLocation) {
    mat4 model;
    const vec3 cPos = { c->cx * CHUNK_SIZE, c->cy * CHUNK_SIZE, c->cz * CHUNK_SIZE };
    glm_translate_make(model, cPos);

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model);
    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    glDrawArrays(GL_TRIANGLES, 0, CHUNK_SIZE_CUBED * 36);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
