#include "chunk.h"
#include <cglm/cglm.h>
#include <logging.h>
#include <string.h>
#include <math.h>

extern void chunk_createMesh(chunk_t *c);

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

void chunk_init(chunk_t *c, int cx, int cy, int cz) {
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
            const float xf = (c->cx * CHUNK_SIZE + x);
            const float zf = (c->cz * CHUNK_SIZE + z);

            const float biome = smoothValueNoise(xf * 0.005f, zf * 0.005f);

            const float n = height(xf, zf);
            const float height = n * 20.f;

            for (int y = 0; y < CHUNK_SIZE; y++) {
                if (c->cy * CHUNK_SIZE + y == (int)height) {
                    ptr[x][y][z] = biome < 0.5f ? BL_GRASS : BL_SAND;
                } else if (c->cy * CHUNK_SIZE + y < height - 6) {
                    ptr[x][y][z] = BL_STONE;
                } else if (c->cy * CHUNK_SIZE + y < height) {
                    ptr[x][y][z] = biome < 0.5f ? BL_DIRT : BL_SAND;
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
