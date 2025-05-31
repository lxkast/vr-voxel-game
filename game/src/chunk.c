#include "chunk.h"
#include <cglm/cglm.h>
#include <logging.h>
#include <string.h>
#include <math.h>
#include "vertices.h"

static float valueNoise(const int x, const int y) {
    int n = x + y * 57;
    n = (n << 13) ^ n;
    const int nn = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return 1.0f - ((float)nn / 1073741824.0f);
}

static float ease(const float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float smoothValueNoise(const float x, const float y) {
    const int x_int = (int)floorf(x);
    const int y_int = (int)floorf(y);
    const float x_frac = ease(x - (float)x_int);
    float y_frac = ease(y - (float)y_int);


    const float v00 = valueNoise(x_int,     y_int);
    const float v10 = valueNoise(x_int + 1, y_int);
    const float v01 = valueNoise(x_int,     y_int + 1);
    const float v11 = valueNoise(x_int + 1, y_int + 1);

    const float i1 = glm_lerp(v00, v10, x_frac);
    const float i2 = glm_lerp(v01, v11, x_frac);
    return glm_lerp(i1, i2, y_frac);
}

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

    int (*ptr)[CHUNK_SIZE][CHUNK_SIZE] = (int (*)[CHUNK_SIZE][CHUNK_SIZE]) c->blocks;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            float xf = (c->cx * CHUNK_SIZE + x) * 0.015f;
            float zf = (c->cz * CHUNK_SIZE + z) * 0.015f;

            float n = smoothValueNoise(xf, zf);
            float height = n * 20.f;

            for (int y = 0; y < CHUNK_SIZE; y++) {
                if (cy * CHUNK_SIZE + y < height) {
                    ptr[x][y][z] = BL_GRASS;
                } else {
                    ptr[x][y][z] = BL_AIR;
                }
            }
        }
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

void chunk_free(const chunk_t *c) {
    glDeleteVertexArrays(1, &c->vbo);
    glDeleteBuffers(1, &c->vao);
}
