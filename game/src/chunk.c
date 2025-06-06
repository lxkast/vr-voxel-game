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

static float smoothValueNoise(const float x, const float y) {
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

static float height(const int x, const int z) {
    float totalHeight = 0.f;
    float frequency = 0.01f;
    float amplitude = 1.f;
    float maxAmplitude = 0.f;

    for (int octave = 0; octave < 4; octave++) {
        totalHeight += smoothValueNoise((float)x * frequency, (float)z * frequency) * amplitude;
        maxAmplitude += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.f;
    }

    return totalHeight / maxAmplitude;
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
                if (c->blocks[i][j][k] == BL_AIR) {
                    continue;
                }
                int ni, nj, nk;
                bool neighbourIsAir = false;
                /*
                    The below is tedious and repetitive, but I didn't
                    give it its own function because it may change significantly
                    when implementing greedy meshing.
                */


                /*
                    back face
                */
                ni = i;
                nj = j;
                nk = k - 1;
                neighbourIsAir = false;
                if (nk < 0) {
                    neighbourIsAir = true;
                } else {
                    neighbourIsAir = c->blocks[ni][nj][nk] == BL_AIR;
                }
                if (neighbourIsAir) {
                    memcpy(nextPtr, backFaceVertices, faceVerticesSize);
                    for (int n = 0; n < 6; n++) {
                        nextPtr[5 * n + 0] += (float)i;
                        nextPtr[5 * n + 1] += (float)j;
                        nextPtr[5 * n + 2] += (float)k;
                        nextPtr[5 * n + 3] = TEXTURE_LENGTH * (nextPtr[5 * n + 3] + c->blocks[i][j][k]) / ATLAS_LENGTH;
                    }
                    nextPtr += faceVerticesSize / sizeof(float);
                }
                /*
                    front face
                */
                ni = i;
                nj = j;
                nk = k + 1;
                neighbourIsAir = false;
                if (nk >= CHUNK_SIZE) {
                    neighbourIsAir = true;
                } else {
                    neighbourIsAir = c->blocks[ni][nj][nk] == BL_AIR;
                }
                if (neighbourIsAir) {
                    memcpy(nextPtr, frontFaceVertices, faceVerticesSize);
                    for (int n = 0; n < 6; n++) {
                        nextPtr[5 * n + 0] += (float)i;
                        nextPtr[5 * n + 1] += (float)j;
                        nextPtr[5 * n + 2] += (float)k;
                        nextPtr[5 * n + 3] = TEXTURE_LENGTH * (nextPtr[5 * n + 3] + c->blocks[i][j][k]) / ATLAS_LENGTH;
                    }
                    nextPtr += faceVerticesSize / sizeof(float);
                }
                /*
                    left face
                */
                ni = i - 1;
                nj = j;
                nk = k;
                neighbourIsAir = false;
                if (ni < 0) {
                    neighbourIsAir = true;
                } else {
                    neighbourIsAir = c->blocks[ni][nj][nk] == BL_AIR;
                }
                if (neighbourIsAir) {
                    memcpy(nextPtr, leftFaceVertices, faceVerticesSize);
                    for (int n = 0; n < 6; n++) {
                        nextPtr[5 * n + 0] += (float)i;
                        nextPtr[5 * n + 1] += (float)j;
                        nextPtr[5 * n + 2] += (float)k;
                        nextPtr[5 * n + 3] = TEXTURE_LENGTH * (nextPtr[5 * n + 3] + c->blocks[i][j][k]) / ATLAS_LENGTH;
                    }
                    nextPtr += faceVerticesSize / sizeof(float);
                }
                /*
                    right face
                */
                ni = i + 1;
                nj = j;
                nk = k;
                neighbourIsAir = false;
                if (ni >= CHUNK_SIZE) {
                    neighbourIsAir = true;
                } else {
                    neighbourIsAir = c->blocks[ni][nj][nk] == BL_AIR;
                }
                if (neighbourIsAir) {
                    memcpy(nextPtr, rightFaceVertices, faceVerticesSize);
                    for (int n = 0; n < 6; n++) {
                        nextPtr[5 * n + 0] += (float)i;
                        nextPtr[5 * n + 1] += (float)j;
                        nextPtr[5 * n + 2] += (float)k;
                        nextPtr[5 * n + 3] = TEXTURE_LENGTH * (nextPtr[5 * n + 3] + c->blocks[i][j][k]) / ATLAS_LENGTH;
                    }
                    nextPtr += faceVerticesSize / sizeof(float);
                }
                /*
                    bottom face
                */
                ni = i;
                nj = j - 1;
                nk = k;
                neighbourIsAir = false;
                if (nj < 0) {
                    neighbourIsAir = true;
                } else {
                    neighbourIsAir = c->blocks[ni][nj][nk] == BL_AIR;
                }
                if (neighbourIsAir) {
                    memcpy(nextPtr, bottomFaceVertices, faceVerticesSize);
                    for (int n = 0; n < 6; n++) {
                        nextPtr[5 * n + 0] += (float)i;
                        nextPtr[5 * n + 1] += (float)j;
                        nextPtr[5 * n + 2] += (float)k;
                        nextPtr[5 * n + 3] = TEXTURE_LENGTH * (nextPtr[5 * n + 3] + c->blocks[i][j][k]) / ATLAS_LENGTH;
                    }
                    nextPtr += faceVerticesSize / sizeof(float);
                }
                /*
                    top face
                */
                ni = i;
                nj = j + 1;
                nk = k;
                neighbourIsAir = false;
                if (nj >= CHUNK_SIZE) {
                    neighbourIsAir = true;
                } else {
                    neighbourIsAir = c->blocks[ni][nj][nk] == BL_AIR;
                }
                if (neighbourIsAir) {
                    memcpy(nextPtr, topFaceVertices, faceVerticesSize);
                    for (int n = 0; n < 6; n++) {
                        nextPtr[5 * n + 0] += (float)i;
                        nextPtr[5 * n + 1] += (float)j;
                        nextPtr[5 * n + 2] += (float)k;
                        nextPtr[5 * n + 3] = TEXTURE_LENGTH * (nextPtr[5 * n + 3] + c->blocks[i][j][k]) / ATLAS_LENGTH;
                    }
                    nextPtr += faceVerticesSize / sizeof(float);
                }
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

void chunk_createDeserialise(chunk_t *c, FILE *fp) {
    fread(&c->cx, sizeof(float), 1, fp);
    fread(&c->cy, sizeof(float), 1, fp);
    fread(&c->cz, sizeof(float), 1, fp);

    fread(&c->blocks, sizeof(int), CHUNK_SIZE_CUBED, fp);

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
            const float xf = (c->cx * CHUNK_SIZE + x);
            const float zf = (c->cz * CHUNK_SIZE + z);

            const float n = height(xf, zf);
            const float height = n * 20.f;

            for (int y = 0; y < CHUNK_SIZE; y++) {
                if (c->cy * CHUNK_SIZE + y == (int)height) {
                    ptr[x][y][z] = BL_GRASS;
                } else if (cy * CHUNK_SIZE + y < height - 20) {
                    ptr[x][y][z] = BL_STONE;
                } else if (cy * CHUNK_SIZE + y < height) {
                    ptr[x][y][z] = BL_DIRT;
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

void chunk_draw(chunk_t *c, const int modelLocation) {
    if (c->tainted) chunk_createMesh(c);
    c->tainted = false;

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

void chunk_serialise(chunk_t *c, FILE *fp) {
    fwrite(&c->cx, sizeof(int), 1, fp);
    fwrite(&c->cy, sizeof(int), 1, fp);
    fwrite(&c->cz, sizeof(int), 1, fp);

    fwrite(&c->blocks, sizeof(int), CHUNK_SIZE_CUBED, fp);
}
