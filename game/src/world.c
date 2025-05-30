#include "world.h"
#include <stdlib.h>
#include "chunk.h"

static bool addChunk(world_t *w, chunk_t *chunk) {
    if (w->chunkN >= MAX_CHUNKS)
        return false;
    w->loadedChunks[w->chunkN++] = chunk;
    return true;
}

static bool removeChunk(world_t *w, const size_t i) {
    if (w->chunkN <= i)
        return false;
    w->loadedChunks[i] = w->loadedChunks[w->chunkN--];
    return true;
}

static bool loadChunk(world_t *w, const int cx, const int cy, const int cz) {
    if (w->chunkN >= MAX_CHUNKS)
        return false;
    chunk_t *chunkPtr = (chunk_t *)malloc(sizeof(chunk_t));
    chunk_create(chunkPtr, cx, cy, cz, BL_GRASS);
    addChunk(w, chunkPtr);
}

static bool unloadChunk(world_t *w, const int cx, const int cy, const int cz) {
    for (int i = 0; i < w->chunkN; i++) {
        chunk_t *chunk = w->loadedChunks[i];
        if (chunk->cx == cx && chunk->cy == cy && chunk->cz == cz) {
            removeChunk(w, i);
            free(chunk);
        }
    }
}

void world_init(world_t *w) {
    w->chunkN = 0;

    for (int i = 0; i < 16; i++)
        loadChunk(w, i % 4 - 2, 0, i / 4 - 2);
}

void world_draw(const world_t *w, const int modelLocation) {
    for (int i = 0; i < w->chunkN; i++) {
        chunk_draw(w->loadedChunks[i], modelLocation);
    }
}

void world_free(world_t *w) {
    for (int i = 0; i < w->chunkN; i++) {
        free(w->loadedChunks[i]);
    }
    w->chunkN = 0;
}
