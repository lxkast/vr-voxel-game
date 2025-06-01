#include "world.h"
#include <cglm/cglm.h>
#include <logging.h>
#include <stdlib.h>
#include "chunk.h"
#include "entity.h"

/**
 * @brief Key for hash table
 */
typedef struct {
    int x, y, z;
} clusterKey_t;

/**
 * @brief Value stored in hashmap entry array
 */
typedef struct {
    /// The pointer to a heap allocated chunk
    chunk_t *chunk;
    /// Whether the chunk will be reloaded this iteration
    bool reloaded;
} chunkValue_t;

/**
 * @brief A cluster and also hashmap
 * @note A cluster is essentially a group of adjacent chunks
 */
typedef struct _s_cluster {
    /// The key to the hashmap entry
    clusterKey_t key;

    /// The heap-allocated array of chunk values in the cluster
    chunkValue_t *cells;
    /// The number of chunks loaded in the clusters
    size_t n;

    /// Hash table marker
    UT_hash_handle hh;
} cluster_t;

/**
 * @brief Maybe gets and maybe creates a cluster and offset from chunk coordinates.
 * @param w A pointer to a world
 * @param cx Chunk x coordinate
 * @param cy Chunk y coordinate
 * @param cz Chunk z coordinate
 * @param create Flag for whether to create the cluster if it doesn't exist
 * @param offset An out parameter for the offset of the given chunk into the cluster
 * @return A pointer to the cluster or null
 */
static cluster_t *clusterGet(world_t *w, const int cx, const int cy, const int cz, bool create, size_t *offset) {
    cluster_t *clusterPtr;

    // Transforming the chunk coordinates to cluster coordinates by
    // removing the 2 LSBs
    clusterKey_t k = {
        cx >> LOG_C_T,
        cy >> LOG_C_T,
        cz >> LOG_C_T,
    };

    // Puts the result of the hash find inside clusterPtr
    HASH_FIND(hh, w->clusterTable, &k, sizeof(clusterKey_t), clusterPtr);

    // Creates the cluster if it doesn't exist, and create is set
    if (clusterPtr) {
    } else {
        if (!create) return 0;

        // Allocate space for the cluster
        clusterPtr = (cluster_t *)calloc(1, sizeof(cluster_t));
        // Allocate space for the cells array inside the cluster
        clusterPtr->cells = calloc(C_T * C_T * C_T, sizeof(chunkValue_t));
        clusterPtr->key = k;

        HASH_ADD(hh, w->clusterTable, key, sizeof(clusterKey_t), clusterPtr);
    }
    // The direct access index of the chunk into the cluster
    *offset =
        (cz - (k.z << LOG_C_T)) * C_T * C_T +
        (cy - (k.y << LOG_C_T)) * C_T +
        (cx - (k.x << LOG_C_T));
    return clusterPtr;
}

/**
 * @brief Loads a chunk.
 * @param w A pointer to a world
 * @param cx Chunk x coordinate
 * @param cy Chunk y coordinate
 * @param cz Chunk z coordinate
 */
static void loadChunk(world_t *w, const int cx, const int cy, const int cz) {
    size_t offset;

    cluster_t *cluster = clusterGet(w, cx, cy, cz, true, &offset);

    chunkValue_t *cv = &cluster->cells[offset];

    if (!cv->chunk) {
        cv->chunk = (chunk_t *)malloc(sizeof(chunk_t));
        chunk_generate(cv->chunk, cx, cy, cz);
        cluster->n++;
    }
    cv->reloaded = true;
}

void world_init(world_t *w) {
    w->clusterTable = NULL;
}

void world_draw(const world_t *w, const int modelLocation) {
    cluster_t *cluster, *tmp;
    HASH_ITER(hh, w->clusterTable, cluster, tmp) {
        for (int i = 0; i < C_T * C_T * C_T; i++)
            if (cluster->cells[i].chunk)
                chunk_draw(cluster->cells[i].chunk, modelLocation);
    }
}

void world_free(world_t *w) {
    cluster_t *cluster, *tmp;

    HASH_ITER(hh, w->clusterTable, cluster, tmp) {
        HASH_DEL(w->clusterTable, cluster);
        for (int i = 0; i < C_T * C_T * C_T; i++) {
            if (!cluster->cells[i].chunk) continue;
            chunk_free(cluster->cells[i].chunk);
            free(cluster->cells[i].chunk);
        }
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
    // Iterate through chunk loaders, loading any chunk in their radius
    for (int i = 0; i < MAX_CHUNK_LOADERS; i++) {
        if (!w->chunkLoaders[i].active)
            continue;
        const int cx = w->chunkLoaders[i].x >> 4;
        const int cy = w->chunkLoaders[i].y >> 4;
        const int cz = w->chunkLoaders[i].z >> 4;

#define CHUNK_LOAD_RADIUS 4
        for (int x = -CHUNK_LOAD_RADIUS; x <= CHUNK_LOAD_RADIUS; x++) {
            for (int y = -CHUNK_LOAD_RADIUS; y <= CHUNK_LOAD_RADIUS; y++) {
                for (int z = -CHUNK_LOAD_RADIUS; z <= CHUNK_LOAD_RADIUS; z++) {
                    if (x * x + y * y + z * z <= CHUNK_LOAD_RADIUS * CHUNK_LOAD_RADIUS) {
                        loadChunk(w, cx + x, cy + y, cz + z);
                    }
                }
            }
        }
    }

    // Iterating through every loaded chunk, if the reloaded flag is false they will be deleted
    cluster_t *cluster, *tmp;
    HASH_ITER(hh, w->clusterTable, cluster, tmp) {
        for (int i = 0; i < C_T * C_T * C_T; i++) {
            if (cluster->cells[i].chunk && !cluster->cells[i].reloaded) {
                chunk_free(cluster->cells[i].chunk);
                free(cluster->cells[i].chunk);
                cluster->cells[i].chunk = NULL;
                cluster->n--;
                if (cluster->n <= 0) {
                    HASH_DEL(w->clusterTable, cluster);
                    free(cluster->cells);
                    free(cluster);
                    break;
                }
            } else {
                cluster->cells[i].reloaded = false;
            }
        }
    }
}

bool world_getBlocki(world_t *w, int x, int y, int z, blockData_t *bd) {
    const int cx = x >> 4;
    const int cy = y >> 4;
    const int cz = z >> 4;

    size_t offset;
    cluster_t *cluster = clusterGet(w, cx, cy, cz, false, &offset);
    if (!cluster) return false;

    const chunkValue_t cv = cluster->cells[offset];
    if (!cv.chunk) return false;

    bd->type = cv.chunk->blocks[x - (cx << 4)][y - (cy << 4)][z - (cz << 4)];
    bd->x = x;
    bd->y = y;
    bd->z = z;

    return true;
}

bool world_getBlock(world_t *w, vec3 pos, blockData_t *bd) {
    const int x = (int)floorf(pos[0]);
    const int y = (int)floorf(pos[1]);
    const int z = (int)floorf(pos[2]);

    return world_getBlocki(w, x, y, z, bd);
}

void world_getAdjacentBlocks(world_t *w, vec3 position, blockData_t *buf) {
    int index = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dz = -1; dz <= 1; dz++) {
                if (dx == 0 && dy == 0 && dz == 0) {
                    continue;
                }
                vec3 delta = {(float)dx, (float)dy, (float)dz};
                vec3 newBlockPosition;
                glm_vec3_add(position, delta, newBlockPosition);

                world_getBlock(w, newBlockPosition, &buf[index]);
                index++;
            }
        }
    }
}

void world_getBlocksInRange(world_t *w, vec3 bottomLeft, const vec3 topRight, blockData_t *buf) {
    int index = 0;

    for (int dx = 0; dx < (int)(topRight[0] - bottomLeft[0]); dx++) {
        for (int dy = 0; dy < (int)(topRight[1] - bottomLeft[1]); dy++) {
            for (int dz = 0; dz < (int)(topRight[2] - bottomLeft[2]); dz++) {
                vec3 delta = {(float)dx, (float)dy, (float)dz};

                vec3 newBlockPosition;

                glm_vec3_add(bottomLeft, delta, newBlockPosition);

                world_getBlock(w, newBlockPosition, &buf[index]);
                index++;
            }
        }
    }
}

bool world_removeBlock(world_t *w, const int x, const int y, const int z) {
    blockData_t bd;
    if (!world_getBlocki(w, x, y, z, &bd)) return false;

    if (bd.type == BL_AIR) return false;

    size_t chunkOffset;
    const cluster_t *cluster = clusterGet(w, x >> 4, y >> 4, z >> 4, false, &chunkOffset);

    chunk_t *chunk = cluster->cells[chunkOffset].chunk;
    chunk->blocks[x - (x << 4)][y - (y << 4)][z - (z << 4)] = BL_AIR;
    chunk->tainted = true;

    return true;
}
