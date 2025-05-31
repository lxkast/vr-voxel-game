#include "chunk.h"
#include <cglm/cglm.h>
#include <logging.h>
#include <string.h>
#include "vertices.h"

/**
 * @brief Creates the mesh from a chunk
 * @param c A pointer to a chunk
 */
static void chunk_createMesh(chunk_t *c) {
    static const size_t bytesPerBlock = sizeof(float) * 36 * 5;

    float *buf = (float *)malloc(CHUNK_SIZE_CUBED * bytesPerBlock);

    float *nextPtr = buf;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                switch (c->blocks[i][j][k]) {
                    case BL_AIR:
                        continue;;
                    case BL_GRASS:
                        memcpy(nextPtr, grassVertices, bytesPerBlock);
                        break;
                    default:
                        LOG_WARN("Undefined block type, treating as air");
                        continue;
                }
                for (int l = 0; l < 36; l++) {
                    nextPtr[5 * l] += 0.5f + i;
                    nextPtr[5 * l + 1] += 0.5f + j;
                    nextPtr[5 * l + 2] += 0.5f + k;
                }
                nextPtr += bytesPerBlock / sizeof(float);
            }
        }
    }

    const GLsizeiptr sizeToWrite = sizeof(float) * (nextPtr - buf);
    c->meshVertices = sizeToWrite / (sizeof(float) * 5);

    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeToWrite, buf, GL_STATIC_DRAW);

    glBindVertexArray(c->vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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
    glGenVertexArrays(1, &c->vao);
    chunk_createMesh(c);
}

void chunk_generate(chunk_t *c, int cx, int cy, int cz) {
    c->cx = cx;
    c->cy = cy;
    c->cz = cz;

    const block_t b = cy > 0 ? BL_AIR : BL_GRASS;

    int *ptr = c->blocks;
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
        ptr[i] = b;
    }

    glGenBuffers(1, &c->vbo);
    glGenVertexArrays(1, &c->vao);
    chunk_createMesh(c);
}

void chunk_draw(const chunk_t *c, const int modelLocation) {
    mat4 model;
    const vec3 cPos = { c->cx * CHUNK_SIZE, c->cy * CHUNK_SIZE, c->cz * CHUNK_SIZE };
    glm_translate_make(model, cPos);

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model);

    glBindVertexArray(c->vao);
    glDrawArrays(GL_TRIANGLES, 0, c->meshVertices);
    glBindVertexArray(0);
}
