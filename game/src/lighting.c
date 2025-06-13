#include <string.h>

#include "lighting.h"
#include "vertices.h"

// performs a BFS flood-fill to approximate the light values of each chunk
void chunk_processLighting(chunk_t *c) {
    while (c->lightQueue.size > 0) {
        lightQueueItem_t head = queue_pop(&c->lightQueue);
        int prevLightValue = c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]];
        if (head.lightValue > prevLightValue) {
            c->lightMap[head.pos[0]][head.pos[1]][head.pos[2]] = head.lightValue > LIGHT_MAX_VALUE ? LIGHT_MAX_VALUE : head.lightValue;
        } else {
            continue;
        }

        // check each direction and add to queue if solid
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
                c->lightMap[nPos[0]][nPos[1]][nPos[2]] < head.lightValue - 1 &&
                head.lightValue - 1 > 0) {
                lightQueueItem_t nItem = { .lightValue = head.lightValue - 1 };
                memcpy(&nItem.pos, &nPos, sizeof(ivec3));
                queue_push(&c->lightQueue, nItem);
            }
        }
    }
}
