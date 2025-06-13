#include <string.h>

#include "lighting.h"
#include "vertices.h"

// performs a BFS flood-fill to approximate the light values of each chunk
void chunk_processLighting(chunk_t *c) {
    while (c->lightQueue.size > 0) {
        lightQueueItem_t head = queue_pop(&c->lightQueue);
        int lightLevel = c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]];

        // check each direction and add to queue if transparent
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

            int newLight = lightLevel - 1;
            if ((c->blocks[nPos[0]][nPos[1]][nPos[2]] == BL_AIR || c->blocks[nPos[0]][nPos[1]][nPos[2]] == BL_LEAF) &&
                newLight > 0 &&
                c->lightMap[nPos[0]][nPos[1]][nPos[2]] < newLight) {
                lightQueueItem_t nItem;
                memcpy(&nItem.pos, &nPos, sizeof(ivec3));
                queue_push(&c->lightQueue, nItem);
                c->lightMap[nPos[0]][nPos[1]][nPos[2]] = newLight;
            }
        }
    }
}
