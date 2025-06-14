#include "chunk.h"
#include <cglm/cglm.h>
#include <logging.h>
#include <string.h>
#include <math.h>
#include "vertices.h"
#include "noise.h"

extern void chunk_createMesh(chunk_t *c);

static float getHumidity(chunk_t *c, const float h, const int x, const int z) {
    const float n = noise_smoothValue(&c->noise, 0.005f * (float)x - 1024.f, 0.005f * (float)z + 1024.f);
    return 20.f * n;
}

static float getTemperature(chunk_t *c, const float h, const int x, const int z) {
    const float n = noise_smoothValue(&c->noise, 0.005f * (float)x + 1024.f, 0.005f * (float)z - 1024.f);
    return 25.f * n;
}

static float getHeight(chunk_t *c, const int x, const int z) {
    const float n = noise_fbm(&c->noise, (float)x, (float)z, 4, 0.5f, 0.01f);
    return (50.f + n * 50.f);
}

void chunk_init(chunk_t *c, const rng_t rng, const noise_t noise, const int cx, const int cy, const int cz) {
    c->rng = rng;
    c->noise = noise;

    c->cx = cx;
    c->cy = cy;
    c->cz = cz;

    glGenBuffers(1, &c->vbo);
    glGenVertexArrays(1, &c->vao);

    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);

    glBindVertexArray(c->vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_INT, 4 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void chunk_fill(chunk_t *c, const block_t block) {
    int *ptr = c->blocks;
    for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; i++) {
        ptr[i] = block;
    }

    c->tainted = true;
}

void chunk_createDeserialise(chunk_t *c, FILE *fp) {
    fread(&c->cx, sizeof(float), 1, fp);
    fread(&c->cy, sizeof(float), 1, fp);
    fread(&c->cz, sizeof(float), 1, fp);

    fread(&c->blocks, sizeof(int), CHUNK_SIZE_CUBED, fp);

    c->tainted = true;
}

void chunk_generate(chunk_t *c) {
    int (*ptr)[CHUNK_SIZE][CHUNK_SIZE] = (int (*)[CHUNK_SIZE][CHUNK_SIZE]) c->blocks;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            const int xg = (c->cx * CHUNK_SIZE + x);
            const int zg = (c->cz * CHUNK_SIZE + z);

            const float height = getHeight(c, xg, zg);
            const float humidityOffset = getHumidity(c, height, xg, zg);
            const float temperatureOffset = getTemperature(c, height, xg, zg);


            for (int y = 0; y < CHUNK_SIZE; y++) {
                const int yg = c->cy * CHUNK_SIZE + y;

                if (yg == (int)height) {
                    const float humidity = 90.f - 0.6f * (float)yg + humidityOffset;
                    const float temperature = 25.f - 0.25f * (float)yg + temperatureOffset;

                    if (temperature > 30.f) {
                        ptr[x][y][z] = BL_SAND;
                    } else if (temperature > 10.f) {
                        ptr[x][y][z] = BL_GRASS;
                    } else if (temperature > 5.f) {
                        ptr[x][y][z] = BL_STONE;
                    } else {
                        ptr[x][y][z] = BL_SNOW;
                    }
                } else if (yg < (int)height - 6) {
                    ptr[x][y][z] = BL_STONE;
                } else if (yg < (int)height) {
                    ptr[x][y][z] = BL_DIRT;
                }
            }
        }
    }

    c->tainted = true;
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
