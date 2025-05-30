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
    w->loadedChunks[i] = w->loadedChunks[--w->chunkN];
    return true;
}

static bool loadChunk(world_t *w, const int cx, const int cy, const int cz) {
    if (w->chunkN >= MAX_CHUNKS)
        return false;
    for (int i = 0; i < w->chunkN; i++) {
        const chunk_t *chunk = w->loadedChunks[i];
        if (chunk->cx == cx && chunk->cy == cy && chunk->cz == cz) {
            return false;
        }
    }
    chunk_t *chunkPtr = (chunk_t *)malloc(sizeof(chunk_t));
    chunk_create(chunkPtr, cx, cy, cz, BL_GRASS);
    addChunk(w, chunkPtr);
    return true;
}

static bool unloadChunk(world_t *w, const int cx, const int cy, const int cz) {
    for (int i = 0; i < w->chunkN; i++) {
        chunk_t *chunk = w->loadedChunks[i];
        if (chunk->cx == cx && chunk->cy == cy && chunk->cz == cz) {
            removeChunk(w, i);
            free(chunk);
            return true;
        }
    }
    return false;
}

void world_init(world_t *w) {
    w->chunkN = 0;
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

bool world_genChunkLoader(world_t *w, unsigned int *id) {
    for (int i = 0; i < MAX_CHUNK_LOADERS; i++) {
        if (w->chunkLoaders[i].active)
            continue;
        *id = i;
        return w->chunkLoaders[i].active = true;
    }
    return false;
}

void world_updateChunkLoader(world_t *w, const unsigned int id, const float pos[3]) {
    w->chunkLoaders[id].x = (int)pos[0];
    w->chunkLoaders[id].y = (int)pos[1];
    w->chunkLoaders[id].z = (int)pos[2];
}

void world_delChunkLoader(world_t *w, const unsigned int id) {
    w->chunkLoaders[id].active = false;
}

void world_doChunkLoading(world_t *w) {
    struct { int x, y, z; } loaded[MAX_CHUNKS];
    size_t next = 0;

    for (int i = 0; i < MAX_CHUNK_LOADERS; i++) {
        if (!w->chunkLoaders[i].active)
            continue;
        const int cx = w->chunkLoaders[i].x / 16;
        const int cy = w->chunkLoaders[i].y / 16;
        const int cz = w->chunkLoaders[i].z / 16;

        static const size_t arr[4][2] = {
            {1, 0}, {0, 1}, {-1, 0}, {0, -1}
        };
        for (int i = 0; i < 4; i++) {
            loadChunk(w, cx + arr[i][0], cy, cz + arr[i][1]);
            loaded[next].x = cx + arr[i][0];
            loaded[next].y = cy;
            loaded[next].z = cz + arr[i][1];
            next++;
        }
    }

    for (int i = 0; i < w->chunkN; i++) {
        bool delete = true;
        for (int j = 0; j < next; j++) {
            if (w->loadedChunks[i]->cx == loaded[j].x &&
                w->loadedChunks[i]->cy == loaded[j].y &&
                w->loadedChunks[i]->cz == loaded[j].z) {
                delete = false;
                break;
            }
        }
        if (delete) {
            unloadChunk(w, w->loadedChunks[i]->cx, w->loadedChunks[i]->cy, w->loadedChunks[i]->cz);
        }
    }
}
