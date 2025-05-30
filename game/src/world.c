#include "world.h"
#include <logging.h>
#include <stdlib.h>
#include "chunk.h"

static cluster_t *clusterGet(world_t *w, const int cx, const int cy, const int cz, bool create, size_t *offset) {
    cluster_t *clusterPtr;
    LOG_DEBUG("Trying to load %d %d %d", cx, cy, cz);

    clusterKey_t k = {
        cx / (1 << LOG_C_T),
        cy / (1 << LOG_C_T),
        cz / (1 << LOG_C_T),
    };

    HASH_FIND(hh, w->clusterTable, &k, sizeof(clusterKey_t), clusterPtr);

    if (clusterPtr) {
        LOG_DEBUG("Cluster %d %d %d exists.", k.x, k.y, k.z);
    } else {
        if (!create) return 0;

        clusterPtr = (cluster_t *)calloc(1,sizeof(cluster_t));
        clusterPtr->cells = calloc(C_T * C_T * C_T, sizeof(chunkValue_t));
        clusterPtr->key = k;

        HASH_ADD(hh, w->clusterTable, key, sizeof(clusterKey_t), clusterPtr);
        LOG_DEBUG("Cluster %d %d %d has been added.", k.x, k.y, k.z);
    }
    *offset =
        (cz - (k.z << LOG_C_T)) * C_T * C_T +
            (cy - (k.y << LOG_C_T)) * C_T +
                (cx - (k.x << LOG_C_T));
    return clusterPtr;
}

static void loadChunk(world_t *w, const int cx, const int cy, const int cz) {
    size_t offset;

    cluster_t *cluster = clusterGet(w, cx, cy, cz, true, &offset);

    chunkValue_t cv = cluster->cells[offset];

    if (!cv.chunk) {
        cv.chunk = (chunk_t *)malloc(sizeof(chunk_t));
        chunk_create(cv.chunk, cx, cy, cz, BL_GRASS);
        cluster->n++;
    }
    cv.reloaded = true;
}

void world_init(world_t *w) {
    w->clusterTable = NULL;
}

void world_draw(const world_t *w, const int modelLocation) {
    cluster_t *cluster, *tmp;
    HASH_ITER(hh, w->clusterTable, cluster, tmp) {
        for (int i = 0; i < C_T * C_T * C_T; i++)
            chunk_draw(cluster->cells[i].chunk, modelLocation);
    }
}

void world_free(world_t *w) {
    cluster_t *cluster, *tmp;

    HASH_ITER(hh, w->clusterTable, cluster, tmp) {
        HASH_DEL(w->clusterTable, cluster);
        for (int i = 0; i < C_T * C_T * C_T; i++)
            free(cluster->cells[i].chunk);
        free(cluster->cells);
        free(cluster);
    }
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
    for (int i = 0; i < MAX_CHUNK_LOADERS; i++) {
        if (!w->chunkLoaders[i].active)
            continue;
        const int cx = w->chunkLoaders[i].x / CHUNK_SIZE;
        const int cy = w->chunkLoaders[i].y / CHUNK_SIZE;
        const int cz = w->chunkLoaders[i].z / CHUNK_SIZE;

#define CHUNK_LOAD_RADIUS 2
        for (int x = -CHUNK_LOAD_RADIUS; x <= CHUNK_LOAD_RADIUS; x++) {
            for (int y = -CHUNK_LOAD_RADIUS; y <= CHUNK_LOAD_RADIUS; y++) {
                for (int z = -CHUNK_LOAD_RADIUS; z <= CHUNK_LOAD_RADIUS; z++) {
                    if (x*x + y*y + z*z <= CHUNK_LOAD_RADIUS * CHUNK_LOAD_RADIUS) {
                        loadChunk(w, cx + x, cy + y, cz + z);
                    }
                }
            }
        }
    }

    cluster_t *cluster, *tmp;
    HASH_ITER(hh, w->clusterTable, cluster, tmp) {
        for (int i = 0; i < C_T * C_T * C_T; i++) {
            if (!cluster->cells[i].reloaded) {
                free(cluster->cells[i].chunk);
                cluster->cells[i].chunk = NULL;
                cluster->n--;
            }
            if (cluster->n <= 0) {
                HASH_DEL(w->clusterTable, cluster);
                free(cluster->cells);
                free(cluster);
            }
        }
    }
}
