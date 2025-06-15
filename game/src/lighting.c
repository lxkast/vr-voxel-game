#include <string.h>

#include "lighting.h"

#include <logging.h>

// computes the light value of a vertex by averaging the 4 light values in the direction of the normal
float computeVertexLight(chunk_t *c, int vx, int vy, int vz, direction_e dir) {
    const int offsets[2] = { 0, -1 };

    int count = 0;
    int sum = 0;

    switch (dir) {
        case DIR_PLUSZ: {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    int nx = vx + offsets[i];
                    int ny = vy + offsets[j];
                    int nz = vz;
                    if (nx >= 0 && nx < CHUNK_SIZE &&
                        ny >= 0 && ny < CHUNK_SIZE &&
                        nz >= 0 && nz < CHUNK_SIZE) {
                        int lv = c->lightMap[nx][ny][nz];
                        sum += lv;
                        count++;
                    } else {
                        // TODO: fetch light value from neighbouring chunk
                    }
                }
            }
            break;
        }
        case DIR_MINUSZ: {
            int dz = -1;
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    int nx = vx + offsets[i];
                    int ny = vy + offsets[j];
                    int nz = vz + dz;
                    if (nx >= 0 && nx < CHUNK_SIZE &&
                        ny >= 0 && ny < CHUNK_SIZE &&
                        nz >= 0 && nz < CHUNK_SIZE) {
                        int lv = c->lightMap[nx][ny][nz];
                        sum += lv;
                        count++;
                    } else {
                        // TODO: fetch light value from neighbouring chunk
                    }
                }
            }
            break;
        }
        case DIR_PLUSY: {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    int nx = vx + offsets[i];
                    int nz = vz + offsets[j];
                    int ny = vy;
                    if (nx >= 0 && nx < CHUNK_SIZE &&
                        ny >= 0 && ny < CHUNK_SIZE &&
                        nz >= 0 && nz < CHUNK_SIZE) {
                        int lv = c->lightMap[nx][ny][nz];
                        sum += lv;
                        count++;
                    } else {
                        // TODO: fetch light value from neighbouring chunk
                    }
                }
            }
            break;
        }
        case DIR_MINUSY: {
            int dy = -1;
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    int nx = vx + offsets[i];
                    int nz = vz + offsets[j];
                    int ny = vy + dy;
                    if (nx >= 0 && nx < CHUNK_SIZE &&
                        ny >= 0 && ny < CHUNK_SIZE &&
                        nz >= 0 && nz < CHUNK_SIZE) {
                        int lv = c->lightMap[nx][ny][nz];
                        sum += lv;
                        count++;
                    } else {
                        // TODO: fetch light value from neighbouring chunk
                    }
                }
            }
            break;
        }
        case DIR_PLUSX: {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    int ny = vy + offsets[i];
                    int nz = vz + offsets[j];
                    int nx = vx;
                    if (nx >= 0 && nx < CHUNK_SIZE &&
                        ny >= 0 && ny < CHUNK_SIZE &&
                        nz >= 0 && nz < CHUNK_SIZE) {
                        int lv = c->lightMap[nx][ny][nz];
                        sum += lv;
                        count++;
                    } else {
                        // TODO: fetch light value from neighbouring chunk
                    }
                }
            }
            break;
        }
        case DIR_MINUSX: {
            int dx = -1;
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    int ny = vy + offsets[i];
                    int nz = vz + offsets[j];
                    int nx = vx + dx;
                    if (nx >= 0 && nx < CHUNK_SIZE &&
                        ny >= 0 && ny < CHUNK_SIZE &&
                        nz >= 0 && nz < CHUNK_SIZE) {
                        int lv = c->lightMap[nx][ny][nz];
                        sum += lv;
                        count++;
                    } else {
                        // TODO: fetch light value from neighbouring chunk
                    }
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
    return (float)sum / (float)count;
}

// performs a BFS flood-fill to approximate the torchlight values of each chunk
static void processTorchLight(chunk_t *c) {
    // propagate darkness
    while (c->lightTorchDeletionQueue.size > 0) {
        lightQueueItem_t head = queue_pop(&c->lightTorchDeletionQueue);
        unsigned char lightLevel = LIGHT_TORCH_MASK & c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]];
        if (lightLevel <= 0) {
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
            for (int i = 0; i < 3; i++) {
                if (nPos[i] < 0 || nPos[i] >= CHUNK_SIZE) {
                    validNeighbour = false;
                    break;
                }
            }
            if (!validNeighbour) {
                continue;
            }
            unsigned char neighbourLight = LIGHT_TORCH_MASK & c->lightMap[nPos[0]][nPos[1]][nPos[2]];
            if ((c->blocks[nPos[0]][nPos[1]][nPos[2]] == BL_AIR || c->blocks[nPos[0]][nPos[1]][nPos[2]] == BL_LEAF)) {
                lightQueueItem_t nItem = { .lightValue = neighbourLight };
                memcpy(&nItem.pos, &nPos, sizeof(ivec3));
                if (neighbourLight < lightLevel && neighbourLight != 0) {
                    queue_push(&c->lightTorchDeletionQueue, nItem);
                } else if (neighbourLight >= lightLevel){
                    queue_push(&c->lightTorchInsertionQueue, nItem);
                }
            }
        }
    }
    // propagate light
    while (c->lightTorchInsertionQueue.size > 0) {
        lightQueueItem_t head = queue_pop(&c->lightTorchInsertionQueue);
        unsigned char lightLevel = LIGHT_TORCH_MASK & c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]];

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

            if ((c->blocks[nPos[0]][nPos[1]][nPos[2]] == BL_AIR || c->blocks[nPos[0]][nPos[1]][nPos[2]] == BL_LEAF) &&
                LIGHT_TORCH_MASK & c->lightMap[nPos[0]][nPos[1]][nPos[2]] < newLight) {
                c->lightMap[nPos[0]][nPos[1]][nPos[2]] =
                    (c->lightMap[nPos[0]][nPos[1]][nPos[2]] & LIGHT_SUN_MASK) | (newLight & LIGHT_TORCH_MASK);
                lightQueueItem_t nItem = { .lightValue = newLight };
                memcpy(&nItem.pos, &nPos, sizeof(ivec3));
                queue_push(&c->lightTorchInsertionQueue, nItem);
            }
        }
    }
}

// performs a BFS flood-fill to approximate the sunlight values of each chunk
static void processSunLight(chunk_t *c) {

}

void chunk_processLighting(chunk_t *c) {
    processTorchLight(c);
    processSunLight(c);
}
