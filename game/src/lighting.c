#include <string.h>

#include "world.h"
#include "lighting.h"

#include <logging.h>

static void sumLight(world_t *w, chunk_t *c, ivec3 nPos, int *sum, int *count) {
    ivec3 chunkOffset = { 0, 0, 0 };
    for (int n = 0; n < 3; ++n) {
        if (nPos[n] < 0) {
            nPos[n] = CHUNK_SIZE - 1;
            chunkOffset[n] = -1;
        } else if (nPos[n] >= CHUNK_SIZE) {
            nPos[n] = 0;
            chunkOffset[n] = 1;
        }
    }
    if (chunkOffset[0] == 0 && chunkOffset[1] == 0 && chunkOffset[2] == 0) {
        int lv = c->lightMap[nPos[0]][nPos[1]][nPos[2]];
        *sum += glm_imax(lv & LIGHT_TORCH_MASK, (lv & LIGHT_SUN_MASK) >> 4);
        (*count)++;
    } else {
        ivec3 cPos = { c->cx, c->cy, c->cz };
        glm_ivec3_add(cPos, chunkOffset, cPos);
        chunk_t *nChunk = world_getFullyLoadedChunk(w, cPos[0], cPos[1], cPos[2]);
        if (nChunk) {
            int lv = nChunk->lightMap[nPos[0]][nPos[1]][nPos[2]];
            *sum += glm_imax(lv & LIGHT_TORCH_MASK, (lv & LIGHT_SUN_MASK) >> 4);
            (*count)++;
        }
    }
}

// computes the light value of a vertex by averaging the 4 light values in the direction of the normal
float computeVertexLight(world_t *w, chunk_t *c, int vx, int vy, int vz, direction_e dir) {
    const int vertexOffset[2] = { 0, -1 };

    int count = 0;
    int sum = 0;

    switch (dir) {
        case DIR_PLUSZ: {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    ivec3 nPos = { vx + vertexOffset[i], vy + vertexOffset[j], vz };
                    sumLight(w, c, nPos, &sum, &count);
                }
            }
            break;
        }
        case DIR_MINUSZ: {
            int dz = -1;
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    int nx = vx + vertexOffset[i];
                    int ny = vy + vertexOffset[j];
                    int nz = vz + dz;
                    ivec3 nPos = { nx, ny, nz };
                    sumLight(w, c, nPos, &sum, &count);
                }
            }
            break;
        }
        case DIR_PLUSY: {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    int nx = vx + vertexOffset[i];
                    int nz = vz + vertexOffset[j];
                    int ny = vy;
                    ivec3 nPos = { nx, ny, nz };
                    sumLight(w, c, nPos, &sum, &count);
                }
            }
            break;
        }
        case DIR_MINUSY: {
            int dy = -1;
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    int nx = vx + vertexOffset[i];
                    int nz = vz + vertexOffset[j];
                    int ny = vy + dy;
                    ivec3 nPos = { nx, ny, nz };
                    sumLight(w, c, nPos, &sum, &count);
                }
            }
            break;
        }
        case DIR_PLUSX: {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    int ny = vy + vertexOffset[i];
                    int nz = vz + vertexOffset[j];
                    int nx = vx;
                    ivec3 nPos = { nx, ny, nz };
                    sumLight(w, c, nPos, &sum, &count);
                }
            }
            break;
        }
        case DIR_MINUSX: {
            int dx = -1;
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    int ny = vy + vertexOffset[i];
                    int nz = vz + vertexOffset[j];
                    int nx = vx + dx;
                    ivec3 nPos = { nx, ny, nz };
                    sumLight(w, c, nPos, &sum, &count);
                }
            }
            break;
        }
        default:
            LOG_FATAL("Invalid direction enum in computeVertexLight");
    }

    if (count == 0) {
        return 0.0f;
    }
    return ((float)sum / (float)count) / (float)LIGHT_MAX_VALUE;
}

// propagate darkness across chunks using a BFS flood fill until queue is empty
static void processTorchLightDeletion(chunk_t *c, world_t *w) {
    while (c->lightTorchDeletionQueue.size > 0) {
        lightQueueItem_t head = queue_pop(&c->lightTorchDeletionQueue);
        unsigned char lightLevel = EXTRACT_TORCH(c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]]);
        if (lightLevel <= 0) {
            continue;
        }
        if (!BL_TRANSPARENT(c->blocks[head.pos[0]][head.pos[1]][head.pos[2]])) {
            continue;
        }
        lightLevel = head.lightValue;
        // set torchlight to 0
        c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]] &= LIGHT_SUN_MASK;

        for (int dir = 0; dir < 6; ++dir) {
            ivec3 dirVec;
            memcpy(&dirVec, &directions[dir], sizeof(ivec3));
            ivec3 nPos;
            glm_ivec3_add(head.pos, dirVec, nPos);

            bool validNeighbour = true;
            ivec3 offset = {0, 0, 0};
            for (int i = 0; i < 3; i++) {
                if (nPos[i] < 0) {
                    offset[i] = -1;
                    nPos[i] = CHUNK_SIZE - 1;
                }
                else if (nPos[i] >= CHUNK_SIZE) {
                    offset[i] = 1;
                    nPos[i] = 0;
                }
            }
            if (offset[0] == 0 && offset[1] == 0 && offset[2] == 0) {
                // propagate darkness within current chunk
                if (BL_TRANSPARENT(c->blocks[nPos[0]][nPos[1]][nPos[2]])) {
                    unsigned char neighbourLight = EXTRACT_TORCH(c->lightMap[nPos[0]][nPos[1]][nPos[2]]);
                    lightQueueItem_t nItem = { .lightValue = neighbourLight };
                    memcpy(&nItem.pos, &nPos, sizeof(ivec3));
                    if (neighbourLight < lightLevel && neighbourLight != 0) {
                        queue_push(&c->lightTorchDeletionQueue, nItem);
                    } else if (neighbourLight >= lightLevel) {
                        queue_push(&c->lightTorchInsertionQueue, nItem);
                    }
                    c->tainted = true;
                }
            } else {
                // propagate darkness within neighbouring chunk
                ivec3 cPos = { c->cx, c->cy, c->cz };
                glm_ivec3_add(cPos, offset, cPos);
                chunk_t *nChunk = world_getFullyLoadedChunk(w, cPos[0], cPos[1], cPos[2]);
                if (nChunk == NULL) {
                    LOG_ERROR("Attempted to propagate torch darkness to unloaded chunk");
                    // TODO: think about what should happen
                } else {
                    if (!BL_TRANSPARENT(nChunk->blocks[nPos[0]][nPos[1]][nPos[2]])) {
                        continue;
                    }
                    unsigned char neighbourLight = EXTRACT_TORCH(nChunk->lightMap[nPos[0]][nPos[1]][nPos[2]]);
                    lightQueueItem_t nItem = { .lightValue = neighbourLight };
                    memcpy(&nItem.pos, &nPos, sizeof(ivec3));
                    if (neighbourLight < lightLevel && neighbourLight != 0) {
                        queue_push(&nChunk->lightTorchDeletionQueue, nItem);
                    } else if (neighbourLight >= lightLevel) {
                        queue_push(&nChunk->lightTorchInsertionQueue, nItem);
                    }
                    nChunk->tainted = true;
                }
            }
        }
    }
}

// propagate light across chunks using a BFS flood fill until queue is empty
static void processTorchLightInsertion(chunk_t *c, world_t *w) {
    // propagate light
    while (c->lightTorchInsertionQueue.size > 0) {
        lightQueueItem_t head = queue_pop(&c->lightTorchInsertionQueue);
        unsigned char lightLevel = LIGHT_TORCH_MASK & c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]];
        if ((c->blocks[head.pos[0]][head.pos[1]][head.pos[2]]) != BL_AIR && c->blocks[head.pos[0]][head.pos[1]][head.pos[2]] != BL_GLOWSTONE) {
            continue;
        }
        if (lightLevel < head.lightValue) {
            c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]] =
                (c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]] & LIGHT_SUN_MASK) | (head.lightValue & LIGHT_TORCH_MASK);
            lightLevel = head.lightValue;
        }

        unsigned char newLight = lightLevel - 1;
        if (newLight <= 0) {
            continue;
        }

        // check each direction and add to queue if transparent and lightValue less than current
        for (int dir = 0; dir < 6; ++dir) {
            ivec3 dirVec;
            memcpy(&dirVec, &directions[dir], sizeof(ivec3));
            ivec3 nPos;
            glm_ivec3_add(head.pos, dirVec, nPos);

            ivec3 offset = {0, 0, 0};
            for (int i = 0; i < 3; i++) {
                if (nPos[i] < 0) {
                    offset[i] = -1;
                    nPos[i] = CHUNK_SIZE - 1;
                }
                else if (nPos[i] >= CHUNK_SIZE) {
                    offset[i] = 1;
                    nPos[i] = 0;
                }
            }
            if (offset[0] == 0 && offset[1] == 0 && offset[2] == 0) {
                // propagate light to current chunk
                if (BL_TRANSPARENT(c->blocks[nPos[0]][nPos[1]][nPos[2]]) &&
                    LIGHT_TORCH_MASK & c->lightMap[nPos[0]][nPos[1]][nPos[2]] < newLight) {
                    c->lightMap[nPos[0]][nPos[1]][nPos[2]] =
                        (c->lightMap[nPos[0]][nPos[1]][nPos[2]] & LIGHT_SUN_MASK) | (newLight & LIGHT_TORCH_MASK);
                    lightQueueItem_t nItem = { .lightValue = newLight };
                    memcpy(&nItem.pos, &nPos, sizeof(ivec3));
                    queue_push(&c->lightTorchInsertionQueue, nItem);
                }
            } else {
                ivec3 cPos = { c->cx, c->cy, c->cz };
                glm_ivec3_add(cPos, offset, cPos);
                chunk_t *nChunk = world_getFullyLoadedChunk(w, cPos[0], cPos[1], cPos[2]);
                if (nChunk == NULL) {
                    nChunk = world_loadChunk(w, cPos[0], cPos[1], cPos[2], LL_INIT, REL_CHILD)->chunk;
                } else {
                    if ((nChunk->blocks[nPos[0]][nPos[1]][nPos[2]]) != BL_AIR) {
                        nChunk->tainted = true;
                        continue;
                    }
                    if (EXTRACT_TORCH(nChunk->lightMap[nPos[0]][nPos[1]][nPos[2]]) >= newLight) {
                        continue;
                    }
                    nChunk->lightMap[nPos[0]][nPos[1]][nPos[2]] =
                        (nChunk->lightMap[nPos[0]][nPos[1]][nPos[2]] & LIGHT_SUN_MASK) | (newLight & LIGHT_TORCH_MASK);
                }
                lightQueueItem_t nItem = { .lightValue = newLight };
                memcpy(&nItem.pos, &nPos, sizeof(ivec3));
                queue_push(&nChunk->lightTorchInsertionQueue, nItem);
            }
        }
    }
}

static void processSunLightInsertion(chunk_t *c, world_t *w) {
    while (c->lightSunInsertionQueue.size > 0) {
        lightQueueItem_t head = queue_pop(&c->lightSunInsertionQueue);
        unsigned char lightLevel = EXTRACT_SUN(c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]]);
        if (c->blocks[head.pos[0]][head.pos[1]][head.pos[2]] != BL_AIR) {
            continue;
        }
        if (lightLevel < head.lightValue) {
            c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]] =
                (c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]] & LIGHT_TORCH_MASK) | ((head.lightValue & LIGHT_TORCH_MASK) << 4);
            lightLevel = head.lightValue;
        }

        if (lightLevel - 1 <= 0) {
            continue;
        }

        // check each direction and add to queue if transparent and lightValue less than current
        // always propagate max sunlight downwards
        for (int dir = 0; dir < 6; ++dir) {
            int newNeighbourLevel;
            if (dir == DIR_MINUSY && lightLevel == LIGHT_MAX_VALUE) {
                newNeighbourLevel = LIGHT_MAX_VALUE;
            } else {
                newNeighbourLevel = lightLevel - 1;
            }
            ivec3 dirVec;
            memcpy(&dirVec, &directions[dir], sizeof(ivec3));
            ivec3 nPos;
            glm_ivec3_add(head.pos, dirVec, nPos);

            bool validNeighbour = true;
            ivec3 offset = {0, 0, 0};
            for (int i = 0; i < 3; i++) {
                if (nPos[i] < 0) {
                    offset[i] = -1;
                    nPos[i] = CHUNK_SIZE - 1;
                }
                else if (nPos[i] >= CHUNK_SIZE) {
                    offset[i] = 1;
                    nPos[i] = 0;
                }
            }
            if (offset[0] == 0 && offset[1] == 0 && offset[2] == 0) {
                // propagate light to current chunk
                if (c->blocks[nPos[0]][nPos[1]][nPos[2]] == BL_AIR &&
                    EXTRACT_SUN(c->lightMap[nPos[0]][nPos[1]][nPos[2]]) < newNeighbourLevel) {
                    c->lightMap[nPos[0]][nPos[1]][nPos[2]] =
                        (c->lightMap[nPos[0]][nPos[1]][nPos[2]] & LIGHT_TORCH_MASK) | ((newNeighbourLevel & LIGHT_TORCH_MASK) << 4);
                    lightQueueItem_t nItem = { .lightValue = newNeighbourLevel };
                    memcpy(&nItem.pos, &nPos, sizeof(ivec3));
                    queue_push(&c->lightSunInsertionQueue, nItem);
                }
            } else {
                // propagate light to neighbouring chunk
                ivec3 cPos = { c->cx, c->cy, c->cz };
                glm_ivec3_add(cPos, offset, cPos);
                chunk_t *nChunk = world_getFullyLoadedChunk(w, cPos[0], cPos[1], cPos[2]);
                if (nChunk == NULL) {
                    continue;
                    nChunk = world_loadChunk(w, cPos[0], cPos[1], cPos[2], LL_INIT, REL_CHILD)->chunk;
                } else {
                    if (nChunk->blocks[nPos[0]][nPos[1]][nPos[2]] != BL_AIR) {
                        nChunk->tainted = true;
                        continue;
                    }
                    if (EXTRACT_SUN(nChunk->lightMap[nPos[0]][nPos[1]][nPos[2]]) >= newNeighbourLevel) {
                        continue;
                    }
                    nChunk->lightMap[nPos[0]][nPos[1]][nPos[2]] =
                            (nChunk->lightMap[nPos[0]][nPos[1]][nPos[2]] & LIGHT_TORCH_MASK) | ((newNeighbourLevel & LIGHT_TORCH_MASK) << 4);
                }
                lightQueueItem_t nItem = { .lightValue = newNeighbourLevel };
                memcpy(&nItem.pos, &nPos, sizeof(ivec3));
                queue_push(&nChunk->lightSunInsertionQueue, nItem);
                nChunk->tainted = true;
            }
        }
    }
}

// performs a BFS flood-fill to approximate the sunlight values of each chunk
static void processSunLightDeletion(chunk_t *c, world_t *w) {
    while (c->lightSunDeletionQueue.size > 0) {
        lightQueueItem_t head = queue_pop(&c->lightSunDeletionQueue);
        unsigned char lightLevel = EXTRACT_SUN(c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]]);
        if (lightLevel <= 0) {
            continue;
        }
        lightLevel = head.lightValue;
        // set sunlight to 0
        c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]] &= LIGHT_TORCH_MASK;

        for (int dir = 0; dir < 6; ++dir) {
            ivec3 dirVec;
            memcpy(&dirVec, &directions[dir], sizeof(ivec3));
            ivec3 nPos;
            glm_ivec3_add(head.pos, dirVec, nPos);

            bool validNeighbour = true;
            for (int i = 0; i < 3; i++) {
                if (nPos[i] < 0 || nPos[i] >= CHUNK_SIZE) {
                    validNeighbour = false;
                    break;
                }
            }
            if (!validNeighbour) {
                continue;
            }

            unsigned char neighbourLight = EXTRACT_SUN(c->lightMap[nPos[0]][nPos[1]][nPos[2]]);
            if ((c->blocks[nPos[0]][nPos[1]][nPos[2]] == BL_AIR || c->blocks[nPos[0]][nPos[1]][nPos[2]] == BL_LEAF)) {
                lightQueueItem_t nItem = { .lightValue = neighbourLight };
                memcpy(&nItem.pos, &nPos, sizeof(ivec3));
                if (dir == DIR_MINUSY || (neighbourLight < lightLevel && neighbourLight != 0)) {
                    queue_push(&c->lightSunDeletionQueue, nItem);
                } else if (neighbourLight >= lightLevel){
                    queue_push(&c->lightSunInsertionQueue, nItem);
                }
            }
        }
    }
}

void chunk_processLightInsertion(chunk_t *c, world_t *w) {
    processTorchLightInsertion(c, w);
    processSunLightInsertion(c, w);
}

void chunk_processLightDeletion(chunk_t *c, world_t *w) {
    processTorchLightDeletion(c, w);
    processSunLightDeletion(c, w);
}