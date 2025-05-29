#include "world.h"
#include "chunk.h"

void world_init(world_t *w) {
    w->chunkN = 0;

    glGenVertexArrays(1, &w->chunkVAO);

    glBindVertexArray(w->chunkVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

bool world_addChunk(world_t *w, const chunk_t chunk) {
    if (w->chunkN >= MAX_CHUNKS)
        return false;
    w->loadedChunks[w->chunkN++] = chunk;
    return true;
}

void world_draw(const world_t *w, const int modelLocation) {
    glBindVertexArray(w->chunkVAO);
    for (int i = 0; i < w->chunkN; i++) {
        chunk_draw(&w->loadedChunks[i], modelLocation);
    }
    glBindVertexArray(0);
}
