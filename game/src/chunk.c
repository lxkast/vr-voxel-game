#include "chunk.h"
#include <cglm/cglm.h>
#include <logging.h>
#include <string.h>
#include <math.h>
#include "vertices.h"
#include "noise.h"

extern void chunk_createMesh(chunk_t *c);

void chunk_init(chunk_t *c, rng_t rng, noise_t noise, int cx, int cy, int cz) {
    c->rng = rng;
    c->noise = noise;

    c->cx = cx;
    c->cy = cy;
    c->cz = cz;
    queue_initQueue(&c->lightTorchInsertionQueue);
    queue_initQueue(&c->lightTorchDeletionQueue);
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

void chunk_generate(chunk_t *c) {
    int (*ptr)[CHUNK_SIZE][CHUNK_SIZE] = (int (*)[CHUNK_SIZE][CHUNK_SIZE]) c->blocks;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            const float xf = (c->cx * CHUNK_SIZE + x);
            const float zf = (c->cz * CHUNK_SIZE + z);

            const float biome = noise_smoothValue(&c->noise, xf * 0.005f, zf * 0.005f);

            const float n = noise_height(&c->noise, xf, zf);
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
    queue_freeQueue(&c->lightTorchInsertionQueue);
    queue_freeQueue(&c->lightTorchDeletionQueue);
}

void chunk_serialise(chunk_t *c, FILE *fp) {
    fwrite(&c->cx, sizeof(int), 1, fp);
    fwrite(&c->cy, sizeof(int), 1, fp);
    fwrite(&c->cz, sizeof(int), 1, fp);

    fwrite(&c->blocks, sizeof(int), CHUNK_SIZE_CUBED, fp);
}
