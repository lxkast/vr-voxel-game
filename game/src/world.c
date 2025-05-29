#include "world.h"
#include "chunk.h"

void world_init(world_t *w) {
    w->chunkN = 0;
}

bool world_addChunk(world_t *w, const chunk_t chunk) {
    if (w->chunkN >= MAX_CHUNKS)
        return false;
    w->loadedChunks[w->chunkN++] = chunk;
    return true;
}

void world_draw(const world_t *w, const int modelLocation) {
    for (int i = 0; i < w->chunkN; i++) {
        chunk_draw(&w->loadedChunks[i], modelLocation);
    }
}
