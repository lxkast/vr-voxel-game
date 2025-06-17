#include "world.h"
#include <cglm/cglm.h>
#include <errno.h>
#include <logging.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "chunk.h"
#include "entity.h"
#include "noise.h"
#include "uthash.h"
#include "vertices.h"
#include "structure.h"
#include "lighting.h"

#define MAX_RAYCAST_DISTANCE 6.f
#define RAYCAST_STEP_MAGNITUDE 0.1f
#define BLOCK_DERENDER_DISTANCE 50.f

/**
 * @brief Key for hash table
 */
typedef struct {
    int x, y, z;
} clusterKey_t;

/**
 * @brief Value stored in hashmap entry array
 */
typedef struct chunkValue_t {
    /// The pointer to a heap allocated chunk
    chunk_t *chunk;
    /// The current level of loading the chunk is in
    chunkLoadLevel_e ll;

    struct {
        reloadData_e reload;
        size_t nChildren;
        chunkValue_t *children[32];
    } loadData;

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

static void world_decorateChunk(world_t *w, chunkValue_t *cv);

/**
 * @brief Loads a chunk.
 * @param w A pointer to a world
 * @param cx Chunk x coordinate
 * @param cy Chunk y coordinate
 * @param cz Chunk z coordinate
 * @param ll The load level to load to if the chunk doesn't exist
 * @param r The reload style of the chunk
 */
static chunkValue_t *world_loadChunk(world_t *w,
                     const int cx,
                     const int cy,
                     const int cz,
                     const chunkLoadLevel_e ll,
                     const reloadData_e r) {
    size_t offset;

    cluster_t *cluster = clusterGet(w, cx, cy, cz, true, &offset);

    chunkValue_t *cv = &cluster->cells[offset];

    if (!cv->chunk) {
        cv->chunk = (chunk_t *)calloc(1, sizeof(chunk_t));

        rng_t chunkRng;
        rng_init(&chunkRng, w->seed ^
                            ((uint64_t)cx << 16) ^
                            ((uint64_t)cy << 32) ^
                            ((uint64_t)cz << 48));

        chunk_init(cv->chunk, chunkRng, w->noise, cx, cy, cz);
        cv->ll = LL_INIT;
        cv->loadData.reload = REL_TOMBSTONE;
        cv->loadData.nChildren = 0;

        cluster->n++;
    }

    if (ll > cv->ll) {
        if (ll > LL_PARTIAL) {
            chunk_generate(cv->chunk);
            world_decorateChunk(w, cv);
        }
        cv->ll = ll;
    }

    if (r < cv->loadData.reload) cv->loadData.reload = r;

    return cv;
}

static bool getBlockAddr(world_t *w, const int x, const int y, const int z, block_t **block, chunk_t **chunk) {
    const int cx = x >> 4;
    const int cy = y >> 4;
    const int cz = z >> 4;

    size_t offset;
    cluster_t *cluster = clusterGet(w, cx, cy, cz, false, &offset);
    if (!cluster) return false;

    const chunkValue_t cv = cluster->cells[offset];
    if (!cv.chunk) return false;

    *chunk = cv.chunk;
    *block = &cv.chunk->blocks[x - (cx << 4)][y - (cy << 4)][z - (cz << 4)];

    return true;
}

static void fogInit(world_t *w, const GLuint program) {
    glUseProgram(program);
    glUniform1f(glGetUniformLocation(program, "fogStart"), FOG_START);
    glUniform1f(glGetUniformLocation(program, "fogEnd"), FOG_END);
    glUseProgram(0);
}

static void highlightInit(world_t *w) {
    glGenVertexArrays(1, &w->highlightVao);
    glGenBuffers(1, &w->highlightVbo);
    glBindVertexArray(w->highlightVao);
    glBindBuffer(GL_ARRAY_BUFFER, w->highlightVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_INT, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (4 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void world_init(world_t *w, GLuint program, uint64_t seed) {
    memset(w, 0, sizeof(world_t));
    w->clusterTable = NULL;
    fogInit(w, program);
    highlightInit(w);

    w->numEntities = 0;
    w->oldestItem = 0;
    w->numPlayers = 0;
    
    w->seed = seed;
    rng_init(&w->worldRng, seed);
    rng_init(&w->generalRng, rng_ull(&w->worldRng));
    w->noise.seed = (uint32_t)rng_ull(&w->worldRng);
}

vec3 chunkBounds = {15.f, 15.f, 15.f};

static bool completelyOutsidePlane(double plane[4], const chunk_t *chunk) {
    double testPoint[3] = {
        (chunk->cx << 4) + (plane[0] > 0 ? 16 : 0),
        (chunk->cy << 4) + (plane[1] > 0 ? 16 : 0),
        (chunk->cz << 4) + (plane[2] > 0 ? 16 : 0),
    };
    double dot = testPoint[0] * plane[0] + testPoint[1] * plane[1] + testPoint[2] * plane[2];
    return dot < -plane[3];
}

// Note - we assume lookVector is normalised
static bool shouldRender(camera_t *cam, const chunk_t *chunk, double planes[6][4]) {
    for (int i = 0; i < 6; i++) {
        if (completelyOutsidePlane(planes[i], chunk)) {
            return false;
        }
    }
    return true;
}

static void calculatePlanes(camera_t *cam, mat4 projection, double res[6][4]) {
    mat4 view;
    camera_createView(cam, view);
    mat4 projview;
    glm_mat4_mul(projection, view, projview);

    for (int i = 0; i < 3; i++) {
        res[i*2][0] = projview[0][3] + projview[0][i];
        res[i*2][1] = projview[1][3] + projview[1][i];
        res[i*2][2] = projview[2][3] + projview[2][i];
        res[i*2][3] = projview[3][3] + projview[3][i];

        res[i*2+1][0] = projview[0][3] - projview[0][i];
        res[i*2+1][1] = projview[1][3] - projview[1][i];
        res[i*2+1][2] = projview[2][3] - projview[2][i];
        res[i*2+1][3] = projview[3][3] - projview[3][i];
    }
}

void world_draw(const world_t *w, const int modelLocation, camera_t *cam, mat4 projection) {
    cluster_t *cluster, *tmp;
    double planes[6][4];
    calculatePlanes(cam, projection, planes);

    HASH_ITER(hh, w->clusterTable, cluster, tmp) {
        for (int i = 0; i < C_T * C_T * C_T; i++) {
            if (!cluster->cells[i].chunk || cluster->cells[i].ll != LL_TOTAL) {continue;}
            const bool renderingChunk = shouldRender(cam, cluster->cells[i].chunk, planes);

            if (cluster->cells[i].chunk && renderingChunk) {
                chunk_draw(cluster->cells[i].chunk, modelLocation);
            }
        }
    }
}

static void freeEntity(const worldEntity_t *e) {
    glDeleteBuffers(1, &e->vbo);
    glDeleteVertexArrays(1, &e->vao);
    free(e->entity);
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

    for (int i = 0; i < w->numEntities; i++) {
        if (w->entities[i].needsFreeing) {
            freeEntity(&w->entities[i]);
        }
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

static bool freeCv(world_t *w, cluster_t *cluster, const int i) {
    chunkValue_t *cv = &cluster->cells[i];

    for (int j = 0; j < cv->loadData.nChildren; j++) {
        reloadData_e *r = &cv->loadData.children[j]->loadData.reload;
        if (*r == REL_CHILD) *r = REL_TOMBSTONE;
    }

    chunk_free(cv->chunk);
    free(cv->chunk);
    cv->chunk = NULL;
    cluster->n--;
    if (cluster->n <= 0) {
        HASH_DEL(w->clusterTable, cluster);
        free(cluster->cells);
        free(cluster);
        return false;
    }
    return true;
}

void world_doChunkLoading(world_t *w) {
    // Iterate through chunk loaders, loading any chunk in their radius
    for (int i = 0; i < MAX_CHUNK_LOADERS; i++) {
        if (!w->chunkLoaders[i].active)
            continue;
        const int cx = w->chunkLoaders[i].x >> 4;
        const int cy = w->chunkLoaders[i].y >> 4;
        const int cz = w->chunkLoaders[i].z >> 4;

        for (int x = -CHUNK_LOAD_RADIUS; x <= CHUNK_LOAD_RADIUS; x++) {
            for (int y = -CHUNK_LOAD_RADIUS; y <= CHUNK_LOAD_RADIUS; y++) {
                for (int z = -CHUNK_LOAD_RADIUS; z <= CHUNK_LOAD_RADIUS; z++) {
                    if (x * x + y * y + z * z <= CHUNK_LOAD_RADIUS * CHUNK_LOAD_RADIUS) {
                        world_loadChunk(w, cx + x, cy + y, cz + z, LL_TOTAL, REL_TOP_RELOAD);
                    }
                }
            }
        }
    }

    // Iterating through every loaded chunk, if the reloaded flag is false they will be deleted
    cluster_t *cluster, *tmp;
    HASH_ITER(hh, w->clusterTable, cluster, tmp) {
        for (int i = 0; i < C_T * C_T * C_T; i++) {
            chunkValue_t *cv = &cluster->cells[i];
            if (!cv->chunk) continue;
            if (cv->loadData.reload == REL_TOP_UNLOAD || cv->loadData.reload == REL_TOMBSTONE) {
                if (!freeCv(w, cluster, i)) break;
            } else if (cv->loadData.reload == REL_TOP_RELOAD) {
                cv->loadData.reload = REL_TOP_UNLOAD;
            }
        }
    }
}

bool world_getBlocki(world_t *w, const int x, const int y, const int z, blockData_t *bd) {
    block_t *block;
    chunk_t *chunk;
    if (!getBlockAddr(w, x, y, z, &block, &chunk)) return false;

    bd->type = *block;
    bd->x = x;
    bd->y = y;
    bd->z = z;

    return true;
}

bool world_getBlock(world_t *w, const vec3 pos, blockData_t *bd) {
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

block_t getBlockType(world_t *w, vec3 position) {
    blockData_t bd;
    if (!world_getBlock(w, position, &bd)) {
        return BL_AIR;
    }
    return bd.type;
}


static void decorator_init(decorator_t *d, chunkValue_t *origin, const int x, const int y, const int z) {
    d->cacheN = 0;
    d->origin = origin;
    memset(d->cache, 0, 27 * sizeof(chunkValue_t *));
    d->cache[1][1][1] = origin;
    d->ox = x;
    d->oy = y;
    d->oz = z;
}

static bool decorator_initSurface(decorator_t *d, chunkValue_t *origin, const int x, const int z, const block_t block) {
    for (int y = CHUNK_SIZE - 1; y >= 0; y--) {
        if (origin->chunk->blocks[x][y][z] == block) {
            decorator_init(d, origin, x, y + 1, z);
            return true;
        }
    }
    return false;
}

static bool decorator_testBlock(decorator_t *d, world_t *world, int x, int y, int z, const block_t match) {
    x = d->ox + x;
    y = d->oy + y;
    z = d->oz + z;

    const int cx = x >> 4;
    const int cy = y >> 4;
    const int cz = z >> 4;

    if (-1 <= cx && cx <= 1 && -1 <= cy && cy <= 1 && -1 <= cz && cz <= 1) {
        chunkValue_t **cacheValue = &d->cache[cx + 1][cy + 1][cz + 1];
        if (!*cacheValue) {
            *cacheValue = world_loadChunk(world,
                                          d->origin->chunk->cx + cx,
                                          d->origin->chunk->cy + cy,
                                          d->origin->chunk->cz + cz,
                                          LL_INIT,
                                          REL_CHILD);
            if (d->origin->loadData.nChildren > 31) {
                LOG_FATAL("Buffer overflow in chunk children");
            }
            bool found = false;
            for (int i = 0; i < d->origin->loadData.nChildren; i++) {
                if (d->origin->loadData.children[i] == *cacheValue) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                d->origin->loadData.children[d->origin->loadData.nChildren++] = *cacheValue;
            }
        }

        const block_t b = (*cacheValue)->chunk->blocks[x - (cx << 4)][y - (cy << 4)][z - (cz << 4)];
        return  b == BL_AIR || b == match;
    }

    return false;
}


static bool world_initStructure(world_t *w, structure_t *structure, chunkValue_t *origin, const int x, const int z, const block_t block) {
    decorator_t d;
    if (!decorator_initSurface(&d, origin, x, z, block)) {
        return false;
    }

    for (int i = 0; i < structure->numBlocks; i++) {
        if (!decorator_testBlock(&d, w, structure->blocks[i].x,
                                        structure->blocks[i].y,
                                        structure->blocks[i].z,
                                        structure->blocks->type)) {
            return false;
        }
    }

    structure->decorator = d;

    return true;
}

static void decorator_placeBlock(decorator_t *d,
                                 world_t *world,
                                 int x,
                                 int y,
                                 int z,
                                 const block_t block,
                                 const float chance) {
    x = d->ox + x;
    y = d->oy + y;
    z = d->oz + z;

    const int cx = x >> 4;
    const int cy = y >> 4;
    const int cz = z >> 4;

    if (-1 <= cx && cx <= 1 && -1 <= cy && cy <= 1 && -1 <= cz && cz <= 1) {
        chunkValue_t **cacheValue = &d->cache[cx + 1][cy + 1][cz + 1];
        if (!*cacheValue) {
            *cacheValue = world_loadChunk(world,
                                          d->origin->chunk->cx + cx,
                                          d->origin->chunk->cy + cy,
                                          d->origin->chunk->cz + cz,
                                          LL_INIT,
                                          REL_CHILD);
            if (d->origin->loadData.nChildren > 31) {
                LOG_FATAL("Buffer overflow in chunk children");
            }
            bool found = false;
            for (int i = 0; i < d->origin->loadData.nChildren; i++) {
                if (d->origin->loadData.children[i] == *cacheValue) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                d->origin->loadData.children[d->origin->loadData.nChildren++] = *cacheValue;
            }
        }

        if (rng_float(&(*cacheValue)->chunk->rng) > chance) {
            return;
        }

        (*cacheValue)->chunk->blocks[x - (cx << 4)][y - (cy << 4)][z - (cz << 4)] = block;
        (*cacheValue)->chunk->tainted = true;
    }
}

static void world_placeStructure(world_t *world, structure_t *structure) {
    for (int i = 0; i < structure->numBlocks; i++) {
        const structureBlock_t block = structure->blocks[i];
        decorator_placeBlock(&structure->decorator, world, block.x, block.y, block.z, block.type, block.chanceToAppear);
    }
}


static void world_decorateChunk(world_t *w, chunkValue_t *cv) {
#define STRUCTURE(type, chance, base)                             \
    for (int x = 0; x < CHUNK_SIZE; x++) {                        \
        for (int z = 0; z < CHUNK_SIZE; z++) {                    \
            if (rng_float(&cv->chunk->rng) < chance) {            \
                structure_t s = type;                             \
                if (world_initStructure(w, &s, cv, x, z, base)) { \
                    world_placeStructure(w, &s);                  \
                }                                                 \
            }                                                     \
        }                                                         \
    }

    if (cv->chunk->biome == BIO_FOREST) {
        STRUCTURE(treeStructure, 0.05f, BL_GRASS);
    }
    if (cv->chunk->biome == BIO_PLAINS) {
        STRUCTURE(treeStructure, 0.001f, BL_GRASS);
    }
    if (cv->chunk->biome == BIO_DESERT) {
        STRUCTURE(cactusStructure, 0.003f, BL_SAND);
    }
    if (cv->chunk->biome == BIO_JUNGLE) {
        STRUCTURE(jungleTreeStructure, 0.05f, BL_JUNGLE_GRASS);
    }
}

static void meshItemEntity(worldEntity_t *e) {
    glGenVertexArrays(1, &e->vao);
    glGenBuffers(1, &e->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, e->vbo);
    glBindVertexArray(e->vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    float *mesh = malloc(itemBlockVerticesSize);
    memcpy(mesh, itemBlockVertices, itemBlockVerticesSize);
    const block_t type = ITEM_TO_BLOCK[e->itemType];
    for (int i = 0; i < 36; ++i) {
        mesh[5 * i + 0] *= e->entity->size[0];
        mesh[5 * i + 1] *= e->entity->size[1];
        mesh[5 * i + 2] *= e->entity->size[2];
        mesh[5 * i + 3] = TEXTURE_LENGTH * (mesh[5 * i + 3] + (float)type) / ATLAS_LENGTH;
    }
    glBufferData(GL_ARRAY_BUFFER, itemBlockVerticesSize, mesh, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    free(mesh);
}


static worldEntity_t createItemEntity(world_t *w, const vec3 pos, const item_e item) {
    worldEntity_t newWorldEntity;
    newWorldEntity.type = WE_ITEM;
    entity_t *newEntity = malloc(sizeof(entity_t));
    newEntity->position[0] = pos[0];
    newEntity->position[1] = pos[1];
    newEntity->position[2] = pos[2];
    newEntity->acceleration[0] = 0;
    newEntity->acceleration[1] = GRAVITY_ACCELERATION;
    newEntity->acceleration[2] = 0;
    newEntity->velocity[0] = rng_floatRange(&w->generalRng, -0.55f, 0.55f);
    newEntity->velocity[1] = 0.5f;
    newEntity->velocity[2] = rng_floatRange(&w->generalRng, -0.55f, 0.55f);
    newEntity->grounded = false;
    newEntity->size[0] = 0.25f;
    newEntity->size[1] = 0.25f;
    newEntity->size[2] = 0.25f;
    newEntity->yaw = 0.f;

    newWorldEntity.entity = newEntity;
    newWorldEntity.itemType = item;
    newWorldEntity.needsFreeing = true;
    meshItemEntity(&newWorldEntity);
    return newWorldEntity;
}


/**
 * @brief Adds an entity to the world
 * @param w A pointer to a world
 * @param entity The world entity to add
 */
void world_addEntity(world_t *w, worldEntity_t entity) {
    if (w->numEntities == MAX_NUM_ENTITIES) {
        for (int i = 0; i < MAX_NUM_ENTITIES; i++) {
            const int entityIndex = (w->oldestItem + i) % w->numEntities;
            if (w->entities[entityIndex].type == WE_ITEM) {
                w->entities[entityIndex] = entity;
                w->entities[entityIndex];
                w->oldestItem = entityIndex+1;
                if (entity.type == WE_PLAYER) {
                    if (w->numPlayers < MAX_NUM_PLAYERS) {
                        w->players[w->numPlayers++] = &w->entities[entityIndex];
                    } else {
                        LOG_FATAL("Max number of players reached, cannot add another");
                    }
                }
                return;
            }
        }
    } else {
        w->entities[w->numEntities++] = entity;
        if (entity.type == WE_PLAYER) {
            if (w->numPlayers < MAX_NUM_PLAYERS) {
                w->players[w->numPlayers++] = &w->entities[w->numEntities-1];
            } else {
                LOG_FATAL("Max number of players reached, cannot add another");
            }
        }
    }
}

void world_removeItemEntity(world_t *w, const int entityIndex) {
    if (entityIndex >= w->numEntities) {
        LOG_FATAL("Entity index out of range");
    }

    if (w->entities[entityIndex].type == WE_ITEM) {
        freeEntity(&w->entities[entityIndex]);
    }

    if (entityIndex == w->numEntities - 1) {
        w->numEntities--;
    } else {
        w->entities[entityIndex] = w->entities[--w->numEntities];
        w->oldestItem = (entityIndex + 1) % w->numEntities;
    }
}

bool world_removeBlock(world_t *w, const int x, const int y, const int z) {
    block_t *bp;
    chunk_t *cp;
    if (!getBlockAddr(w, x, y, z, &bp, &cp)) return false;

    if (*bp == BL_AIR) return false;

    ivec3 blockPos = { x - ((x >> 4) << 4), y - ((y >> 4) << 4), z - ((z >> 4) << 4) };

    unsigned char torchValue = EXTRACT_TORCH(cp->lightMap[blockPos[0]][blockPos[1]][blockPos[2]]);

    if (*bp == BL_GLOWSTONE) {
        lightQueueItem_t qi = {
            .pos = { x - ((x >> 4) << 4), y - ((y >> 4) << 4), z - ((z >> 4) << 4) },
            .lightValue = torchValue };
        queue_push(&cp->lightTorchDeletionQueue, qi);
    }
    if (*bp != BL_LEAF) {
        for (int dir = 0; dir < 6; ++dir) {
            ivec3 nPos;
            memcpy(nPos, directions[dir], sizeof(ivec3));
            glm_ivec3_add(nPos, blockPos, nPos);
            if (nPos[0] < 0 || nPos[0] >= CHUNK_SIZE ||
                nPos[1] < 0 || nPos[1] >= CHUNK_SIZE ||
                nPos[2] < 0 || nPos[2] >= CHUNK_SIZE) {
                continue;
            }
            unsigned char neighborLightMapValue = cp->lightMap[nPos[0]][nPos[1]][nPos[2]];
            unsigned char neighborSun = EXTRACT_SUN(neighborLightMapValue);
            unsigned char neighborTorch = EXTRACT_TORCH(neighborLightMapValue);

            if (neighborSun > 0) {
                queue_push(&cp->lightSunInsertionQueue, (lightQueueItem_t){ .pos = {nPos[0], nPos[1], nPos[2]}, .lightValue = neighborSun });
            }
            if (neighborTorch > 0) {
                queue_push(&cp->lightTorchInsertionQueue, (lightQueueItem_t){ .pos = {nPos[0], nPos[1], nPos[2]}, .lightValue = neighborTorch });
            }
        }
    }


    const worldEntity_t entity = createItemEntity(w, (vec3){(float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f}, BLOCK_TO_ITEM[*bp]);
    world_addEntity(w, entity);

    *bp = BL_AIR;
    cp->tainted = true;

    return true;
}

bool world_placeBlock(world_t *w, const int x, const int y, const int z, const block_t block) {
    block_t *bp;
    chunk_t *cp;
    if (!getBlockAddr(w, x, y, z, &bp, &cp)) return false;

    if (*bp != BL_AIR) {
        return false;
    }

    ivec3 blockPos = { x - ((x >> 4) << 4), y - ((y >> 4) << 4), z - ((z >> 4) << 4) };

    if (block == BL_GLOWSTONE) {
        lightQueueItem_t qi = {
            .lightValue = LIGHT_MAX_VALUE };
        memcpy(&qi.pos, &blockPos, sizeof(ivec3));
        queue_push(&cp->lightTorchInsertionQueue, qi);
    }
    int sunValue = EXTRACT_SUN(cp->lightMap[blockPos[0]][blockPos[1]][blockPos[2]]);
    int torchValue = EXTRACT_TORCH(cp->lightMap[blockPos[0]][blockPos[1]][blockPos[2]]);
    if (sunValue > 0 && block != BL_LEAF) {
        lightQueueItem_t qi = {
            .lightValue = sunValue };
        memcpy(&qi.pos, &blockPos, sizeof(ivec3));
        queue_push(&cp->lightSunDeletionQueue, qi);
    }
    if (torchValue > 0 && block != BL_LEAF) {
        lightQueueItem_t qi = {
            .lightValue = torchValue };
        memcpy(&qi.pos, &blockPos, sizeof(ivec3));
        queue_push(&cp->lightTorchDeletionQueue, qi);
    }
    *bp = block;
    cp->tainted = true;

    return true;
}

bool world_save(const world_t *w, const char *dir) {
    struct stat st = {0};
    if (stat(dir, &st) == -1) {
        LOG_INFO("World save does not exist, creating directory...");
#if defined(_WIN32) || defined(_WIN64)
        if (mkdir(dir) != 0) {
#else
        if (mkdir(dir, 0777) != 0) {
#endif
            LOG_ERROR("Failed to create world directory: %s", strerror(errno));
            return false;
        }
    }

    const size_t dirLen = strlen(dir);
    char *nameBuf = (char *)malloc(dirLen + 64);
    block_t *empty = (block_t *)malloc(sizeof(block_t) * CHUNK_SIZE_CUBED);

    cluster_t *cluster, *tmp;
    HASH_ITER(hh, w->clusterTable, cluster, tmp) {
        sprintf(nameBuf, "%s%d %d %d.cluster", dir, cluster->key.x, cluster->key.y, cluster->key.z);

        FILE *fp = fopen(nameBuf, "wb");
        if (!fp) {
            LOG_ERROR("Failed to open cluster file: %s", strerror(errno));
            free(empty);
            free(nameBuf);
            return false;
        }

        static char valid = 0;
        for (int i = 0; i < C_T * C_T * C_T; i++) {
            chunk_t *chunk = cluster->cells[i].chunk;
            if (!chunk) {
                valid = 0;
                fwrite(&valid, 1, 1, fp);
                fwrite(empty, sizeof(block_t), CHUNK_SIZE_CUBED, fp);
            } else {
                valid = 1;
                fwrite(&valid, 1, 1, fp);
                chunk_serialise(chunk, fp);
            }
        }

        fclose(fp);
    }

    free(empty);
    free(nameBuf);
    return true;
}

raycast_t world_raycast(world_t *w, vec3 startPosition, vec3 viewDirection, const float raycastDistance) {
    vec3 viewNormalised;
    glm_vec3_copy(viewDirection, viewNormalised);
    glm_normalize(viewNormalised);

    vec3 currentBlock;
    glm_vec3_floor(startPosition, currentBlock);

    // stores the amount we must move along the ray to get to the next edge
    // in each direction
    vec3 axisMoveDelta;

    axisMoveDelta[0] = (viewNormalised[0] == 0) ? 1e5f : fabsf(1 / viewNormalised[0]);
    axisMoveDelta[1] = (viewNormalised[1] == 0) ? 1e5f : fabsf(1 / viewNormalised[1]);
    axisMoveDelta[2] = (viewNormalised[2] == 0) ? 1e5f : fabsf(1 / viewNormalised[2]);

    // calculates which direction we move in along each axis
    vec3 stepDirection;
    stepDirection[0] = viewNormalised[0] < 0 ? -1.0f : 1.0f;
    stepDirection[1] = viewNormalised[1] < 0 ? -1.0f : 1.0f;
    stepDirection[2] = viewNormalised[2] < 0 ? -1.0f : 1.0f;

    // Calculating initial distances to next block
    vec3 distToNextBlock;

    distToNextBlock[0] = viewNormalised[0] < 0 ? startPosition[0] - currentBlock[0] : currentBlock[0] + 1 - startPosition[0];
    distToNextBlock[1] = viewNormalised[1] < 0 ? startPosition[1] - currentBlock[1] : currentBlock[1] + 1 - startPosition[1];
    distToNextBlock[2] = viewNormalised[2] < 0 ? startPosition[2] - currentBlock[2] : currentBlock[2] + 1 - startPosition[2];

    distToNextBlock[0] *= axisMoveDelta[0];
    distToNextBlock[1] *= axisMoveDelta[1];
    distToNextBlock[2] *= axisMoveDelta[2];

    float totalDistance = 0;

    raycastFace_e currentFace = POS_X_FACE;

    // Check starting block first
    if (getBlockType(w, currentBlock) != BL_AIR) {
        return (raycast_t){
            .blockPosition = {currentBlock[0], currentBlock[1], currentBlock[2]},
            .face = currentFace,
            .found = true
        };
    }

    while (totalDistance < raycastDistance) {
        // checks for a solid block
        if (getBlockType(w, currentBlock) != BL_AIR) {
            return (raycast_t){
                .blockPosition = {currentBlock[0], currentBlock[1], currentBlock[2]},
                .face = currentFace,
                .found = true
            };
        }

        // steps to the next closest block
        if (distToNextBlock[0] < distToNextBlock[1] && distToNextBlock[0] < distToNextBlock[2]) {
            // Step in X direction
            totalDistance = distToNextBlock[0];
            distToNextBlock[0] += axisMoveDelta[0];
            currentBlock[0] += stepDirection[0];
            if (stepDirection[0] == 1) {
                currentFace = NEG_X_FACE;
            } else {
                currentFace = POS_X_FACE;
            }
        } else if (distToNextBlock[1] < distToNextBlock[2]) {
            // Step in Y direction
            totalDistance = distToNextBlock[1];
            distToNextBlock[1] += axisMoveDelta[1];
            currentBlock[1] += stepDirection[1];
            if (stepDirection[1] == 1) {
                currentFace = NEG_Y_FACE;
            } else {
                currentFace = POS_Y_FACE;
            }
        } else {
            // Step in Z direction
            totalDistance = distToNextBlock[2];
            distToNextBlock[2] += axisMoveDelta[2];
            currentBlock[2] += stepDirection[2];
            if (stepDirection[2] == 1) {
                currentFace = NEG_Z_FACE;
            } else {
                currentFace = POS_Z_FACE;
            }
        }
    }

    return (raycast_t){
        .blockPosition = {0, 0, 0},
        .face = POS_X_FACE,
        .found = false
    };
}

void world_highlightFace(world_t *w, camera_t *camera) {
    vec3 ray;
    glm_vec3_scale(camera->ruf[2], -1.0f, ray);

    raycast_t res = world_raycast(w, camera->eye, ray, MAX_RAYCAST_DISTANCE);
    w->highlightFound = res.found;
    if (!res.found) {
        return;
    }
    vertex_t *buffer = malloc(faceVerticesSize);

    vec3 delta = { 0.f, 0.f, 0.f };
    memcpy(buffer, blockVertices[res.face], faceVerticesSize);
    switch (res.face) {
        case POS_X_FACE:
            delta[0] += 0.001f;
            break;
        case NEG_X_FACE:
            delta[0] -= 0.001f;
            break;
        case POS_Y_FACE:
            delta[1] += 0.001f;
            break;
        case NEG_Y_FACE:
            delta[1] -= 0.001f;
            break;
        case POS_Z_FACE:
            delta[2] += 0.001f;
            break;
        case NEG_Z_FACE:
            delta[2] -= 0.001f;
            break;
        default: LOG_FATAL("invalid face type");
    }

    glm_vec3_add(res.blockPosition, delta, res.blockPosition);

    glm_translate_make(w->highlightModel, res.blockPosition);

    glBindBuffer(GL_ARRAY_BUFFER, w->highlightVbo);
    glBufferData(GL_ARRAY_BUFFER, faceVerticesSize, buffer, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    free(buffer);
}

void world_drawHighlight(const world_t *w, const int modelLocation) {
    if (!w->highlightFound) {
        return;
    }
    glBindVertexArray(w->highlightVao);

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, w->highlightModel);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void world_processAllEntities(world_t *w, const double dt) {
    for (int i = 0; i < w->numEntities; i++) {
        if (w->entities[i].type != WE_NONE) {
            if (w->entities[i].type == WE_ITEM) {
                for (int j = 0; j < w->numPlayers; j++) {
                    if (glm_vec3_distance(w->entities[i].entity->position, w->players[j]->entity->position) > BLOCK_DERENDER_DISTANCE) {
                        world_removeItemEntity(w, i);
                        i--;
                    }
                }
            }
            w->entities[i].entity->acceleration[1] = GRAVITY_ACCELERATION;
            processEntity(w, w->entities[i].entity, dt);
        }
    }
}

void world_drawAllEntities(const world_t *w, const int modelLocation) {
    for (int i = 0; i < w->numEntities; i++) {
        if (w->entities[i].type == WE_ITEM) {
            mat4 model;
            glm_translate_make(model, w->entities[i].entity->position);
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model);
            glBindVertexArray(w->entities[i].vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    }
}