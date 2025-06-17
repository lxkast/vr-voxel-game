#include "chunk.h"
#include <cglm/cglm.h>
#include <logging.h>
#include <string.h>
#include <math.h>
#include "vertices.h"
#include "noise.h"

#define LIGHT_MAX_VALUE 15

extern void chunk_createMesh(chunk_t *c);

static float smoothstep(const float min, const float max, float x) {
    x = glm_clamp((x - min) / (max - min), 0.f, 1.f);
    return x * x * (3.0f - 2.0f * x);
}

static float getHumidity(chunk_t *c, const float h, const int x, const int z) {
    const float n = noise_smoothValue(&c->noise, 0.005f * (float)x - 1024.f, 0.005f * (float)z + 1024.f);
    return 20.f * n;
}

static float getTemperature(chunk_t *c, const float h, const int x, const int z) {
    const float n = noise_smoothValue(&c->noise, 0.003f * (float)x + 1024.f, 0.003f * (float)z - 1024.f);
    return 15.f * n;
}

static float getHeight(chunk_t *c, const int x, const int z) {
    const float xf = (float)x;
    const float zf = (float)z;

    const float biome = 0.5f + (0.5f * noise_fbm(&c->noise, xf, zf, 2, 0.5f, 0.005f));
    const float biomeMask = smoothstep(0.4f, 0.8f, biome);

    const float hills = 0.5f + (0.5f * noise_fbm(&c->noise, xf, zf, 5, 0.4f, 0.01f));

    const float flat = 0.1f + (0.1f * noise_fbm(&c->noise, xf, zf, 3, 0.4f, 0.01f));

    const float h = glm_lerp(flat, hills, biomeMask);

    return h * 50.f;
}

struct biomeSlice {
    int height;
    float humidityOffset;
    float temperatureOffset;

    int x, z;
};

static struct biomeSlice createBiomeSlice(chunk_t *c, const int x, const int z) {
    const float h = getHeight(c, x, z);
    return (struct biomeSlice) {
        .height = (int)h,
        .humidityOffset = getHumidity(c, h, x, z),
        .temperatureOffset = getTemperature(c, h, x, z),
        .x = x,
        .z = z
    };
}

static biome_e getBiome(chunk_t *c, const struct biomeSlice bs, const int y) {
    const float humidity = 70.f - 0.6f * (float)y + bs.humidityOffset;
    const float temperature = 30.f - 0.50f * (float)y + bs.temperatureOffset;

    if (bs.height - y > 5) {
        return BIO_CAVE;
    }
    if (temperature > 30.f) {
        if (humidity > 75.f) {
            return BIO_JUNGLE;
        }
        return BIO_DESERT;
    }
    if (temperature > 15.f) {
        if (y > 15) {
            return BIO_FOREST;
        }
        return BIO_PLAINS;
    }

    return BIO_TUNDRA;
}


void chunk_init(chunk_t *c, const rng_t rng, const noise_t noise, const int cx, const int cy, const int cz) {
    c->rng = rng;
    c->noise = noise;
    c->biome = BIO_NIL;

    c->cx = cx;
    c->cy = cy;
    c->cz = cz;
    queue_initQueue(&c->lightTorchInsertionQueue);
    queue_initQueue(&c->lightTorchDeletionQueue);
    queue_initQueue(&c->lightSunInsertionQueue);
    queue_initQueue(&c->lightSunDeletionQueue);
    memset(c->lightMap, 0, CHUNK_SIZE_CUBED * sizeof(unsigned char));

    glGenBuffers(1, &c->vbo);
    glGenVertexArrays(1, &c->vao);
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

void chunk_initSun(chunk_t *c) {
    return;
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        for (int j = 0; j < CHUNK_SIZE; ++j) {
            if (c->blocks[i][CHUNK_SIZE - 1][j] == BL_AIR || c->blocks[i][CHUNK_SIZE - 1][j] == BL_LEAF) {
                lightQueueItem_t nItem = { .pos = { i, CHUNK_SIZE - 1, j }, .lightValue = LIGHT_MAX_VALUE };
                queue_push(&c->lightSunInsertionQueue, nItem);
            }
        }
    }
}

void chunk_generate(chunk_t *c) {
    int (*ptr)[CHUNK_SIZE][CHUNK_SIZE] = (int (*)[CHUNK_SIZE][CHUNK_SIZE]) c->blocks;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            const int xg = (c->cx * CHUNK_SIZE + x);
            const int zg = (c->cz * CHUNK_SIZE + z);

            struct biomeSlice bs = createBiomeSlice(c, xg, zg);

            for (int y = 0; y < CHUNK_SIZE; y++) {
                const int yg = c->cy * CHUNK_SIZE + y;

                const int ds = bs.height - yg;
                if (ds >= 0) {
                    const biome_e b = getBiome(c, bs, yg);
                    switch (b) {
                        case BIO_NIL: {
                            LOG_WARN("Unknown biome");
                            ptr[x][y][z] = BL_STONE;
                            break;
                        }
                        case BIO_PLAINS:
                        case BIO_FOREST: {
                            ptr[x][y][z] = ds == 0 ? BL_GRASS : BL_DIRT;
                            break;
                        }
                        case BIO_JUNGLE: {
                            ptr[x][y][z] = ds == 0 ? BL_JUNGLE_GRASS : BL_MUD;
                            break;
                        }
                        case BIO_DESERT: {
                            ptr[x][y][z] = BL_SAND;
                            break;
                        }
                        case BIO_TUNDRA: {
                            ptr[x][y][z] = BL_SNOW;
                            break;
                        }
                        case BIO_CAVE: {
                            ptr[x][y][z] = BL_STONE;
                            break;
                        }
                    }
                    if (x == 7 && z == 7 && ds == 0) {
                        c->biome = b;
                    }
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
    queue_freeQueue(&c->lightTorchInsertionQueue);
    queue_freeQueue(&c->lightTorchDeletionQueue);
    queue_freeQueue(&c->lightSunInsertionQueue);
    queue_freeQueue(&c->lightSunDeletionQueue);
}

void chunk_serialise(chunk_t *c, FILE *fp) {
    fwrite(&c->cx, sizeof(int), 1, fp);
    fwrite(&c->cy, sizeof(int), 1, fp);
    fwrite(&c->cz, sizeof(int), 1, fp);

    fwrite(&c->blocks, sizeof(int), CHUNK_SIZE_CUBED, fp);
}
