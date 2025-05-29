#pragma once

#include <glad/gl.h>
#include <stdbool.h>
#include "block.h"

#define CHUNK_SIZE 16
#define CHUNK_SIZE_CUBED 4096

typedef struct {
    int cx, cy, cz;
    block_t blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    GLuint vbo;
    bool tainted;
} chunk_t;

void chunk_create(chunk_t *c, int cx, int cy, int cz, block_t block);

void chunk_draw(const chunk_t *c, const int modelLocation);
