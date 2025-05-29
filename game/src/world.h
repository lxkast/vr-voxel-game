#pragma once

#include <glad/gl.h>
#include <stddef.h>
#include "chunk.h"

#define MAX_CHUNKS 16

typedef struct {
    chunk_t loadedChunks[MAX_CHUNKS];
    size_t chunkN;
} world_t;

void world_init(world_t *w);

bool world_addChunk(world_t *w, chunk_t chunk);

void world_draw(const world_t *w, int modelLocation);
