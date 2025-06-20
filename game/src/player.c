#include "player.h"
#include "GLFW/glfw3.h"
#include "logging.h"
#include "string.h"
#include "block.h"
#include "world.h"

static const int faceToBlock[6][3] = {{-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1} };

/**
 * @brief Sets a player's block cooldown time.
 * @param p A pointer to a player
 */
static void setBlockCooldown(player_t *p) {
    p->blockCooldown = glfwGetTime() + BLOCK_COOLDOWN_TIME;
}

/**
 * @brief Checks if a player is currently on block placing/destroying cooldown
 * @param p A pointer to a player
 * @return Whether the player is or isn't on cooldown
 */
static bool onBlockCooldown(const player_t *p) {
    return glfwGetTime() < p->blockCooldown;
}

/**
 * @brief Adds a player to a world as an entity
 * @param p A pointer to a player
 * @param w A pointer to a world
 */
static void player_addToWorld(player_t *p, world_t *w) {
    world_addEntity(w, (worldEntity_t){
        .type = WE_PLAYER,
        .entity = &p->entity,
        .itemType = -1,
        .needsFreeing = false,
        .vao = -1,
        .vbo = -1
    });
}

void player_init(world_t *w, player_t *p) {
    vec3 start = {0.f, 100.f, 0.f};

    while (true) {
        blockData_t bd;
        world_getBlock(w, start, &bd);
        if (bd.type != BL_AIR) {
            break;
        } else {
            start[1]--;
        }
    }
    *p = (player_t){
        .entity = {
            .position = {start[0], start[1]+1.2f, start[2]},
            .velocity = {0.f, 0.f, 0.f},
            .size = {0.6f, 1.85f, 0.6f},
            .acceleration = {0.f, 0.f, 0.f},
            .grounded = false,
            .yaw = 0,
        },
        .lookVector = { 0.f, 0.f, 0.f },
        .cameraOffset = {0.3f, 1.8f, 0.3f},
        .hotbar = {
            .slots = {
                {ITEM_DIRT, 64},
                {ITEM_GRASS, 32},
                {ITEM_STONE, 16},
                {ITEM_GLOWSTONE, 64},
                {ITEM_SNOW, 64},
                {NOTHING, 0},
                {NOTHING, 0},
                {NOTHING, 0},
                {NOTHING, 0}
            },
            .currentSlotIndex = 0
        }
    };

    p->hotbar.currentSlot = &(p->hotbar.slots[0]);
    player_addToWorld(p, w);
}

void player_attachCamera(player_t *p, camera_t *camera) {
    vec3 camPos;
    glm_vec3_add(p->entity.position, p->cameraOffset, camPos);
    camera_setPos(camera, camPos);

    vec3 blockPosition;
    vec3 sub1 = {0.f, 1.f, 0.f};
    glm_vec3_sub(p->entity.position, sub1, blockPosition);
    glm_vec3_floor(blockPosition, blockPosition);

    const float qx = camera->ori[0];
    const float qy = camera->ori[1];
    const float qz = camera->ori[2];
    const float qw = camera->ori[3];

    const float sinyCosp = 2.0f * (qw * qy + qx * qz);
    const float cosyCosp = 1.0f - 2.0f * (qy * qy + qz * qz);
    const float yaw = atan2f(sinyCosp, cosyCosp);

    p->entity.yaw = yaw;
    glm_vec3_copy(camera->ruf[2], p->lookVector);
}

void player_removeBlock(player_t *p, world_t *w) {
    if (onBlockCooldown(p)) {
        return;
    }

    vec3 camPos;

    glm_vec3_add(p->entity.position, p->cameraOffset, camPos);

    // remove minus sign later
    vec3 lookVector;
    glm_vec3_scale(p->lookVector, -1, lookVector);

    const raycast_t raycastBlock = world_raycast(w, camPos, lookVector, 6.f);

    if (raycastBlock.found) {
        world_removeBlock(w,
                          (int)raycastBlock.blockPosition[0],
                          (int)raycastBlock.blockPosition[1],
                          (int)raycastBlock.blockPosition[2]);

        setBlockCooldown(p);
    }
}

void player_placeBlock(player_t *p, world_t *w) {
    if (onBlockCooldown(p) || ITEM_PROPERTIES[p->hotbar.currentSlot->type].isPlaceable == false) {
        return;
    }
    vec3 camPos;

    glm_vec3_add(p->entity.position, p->cameraOffset, camPos);

    // remove minus sign later
    vec3 lookVector;
    glm_vec3_scale(p->lookVector, -1, lookVector);

    const raycast_t raycastBlock = world_raycast(w, camPos, lookVector, 6.f);

    if (raycastBlock.found) {
        const int *moveDelta = faceToBlock[raycastBlock.face];

        ivec3 newBlockPosition;

        newBlockPosition[0] = (int)raycastBlock.blockPosition[0] - moveDelta[0];
        newBlockPosition[1] = (int)raycastBlock.blockPosition[1] - moveDelta[1];
        newBlockPosition[2] = (int)raycastBlock.blockPosition[2] - moveDelta[2];

        if (!intersectsWithBlock(p->entity, newBlockPosition)) {
            world_placeBlock(w,
                             newBlockPosition[0],
                             newBlockPosition[1],
                             newBlockPosition[2],
                             ITEM_TO_BLOCK[p->hotbar.currentSlot->type]);
            setBlockCooldown(p);
            p->hotbar.currentSlot->count--;
            if (p->hotbar.currentSlot->count == 0) {
                p->hotbar.currentSlot->type = NOTHING;
            }

            player_printHotbar(p);
        }
   }
}

/**
 * @brief Prints a character a certain number of time
 * @param ch The character to repeat
 * @param num The number of times to repeat it
 */
static void repN(const char ch, const unsigned long long num) {
    for (int i = 0; i < num; i++) {
        putchar(ch);
    }
}

/**
 * @brief This displays the player's hotbar in the terminal. This is so
 *        the code can be tested before we implement the hotbar visually.
 * @param p the player whose hotbar we want to print
 */
void player_printHotbar(const player_t *p) {
    char printStrings[9][30];

    // working out the text for each slot, and how long it will be
    for (int i = 0; i < 9; i++) {
        char *printStr = printStrings[i];
        const hotbarItem_t item = p->hotbar.slots[i];
        if (item.type != NOTHING) {
            strcpy(printStr, ITEM_PROPERTIES[item.type].displayName);
            strcat(printStr, " x");
            char countStr[3];
            snprintf(countStr, sizeof(countStr), "%d", item.count);
            strcat(printStr, countStr);
        } else {
            strcpy(printStr, "   ");
        }
    }


    // printing line above
    putchar('+');
    for (int i = 0; i < 9; i++) {
        repN('-', 2 + strlen(printStrings[i]));
        putchar('+');
    }
    printf("\n|");

    // printing slots
    for (int i = 0; i < 9; i++) {
        printf(" %s |", printStrings[i]);
    }

    // printing line below
    printf("\n+");
    for (int i = 0; i < 9; i++) {
        repN('-', 2 + strlen(printStrings[i]));
        putchar('+');
    }
    printf("\n");
}

void player_pickUpItemsCheck(player_t *p, world_t *w) {
    for (int i = 0; i < w->numEntities; i++) {
        const worldEntity_t worldEntity = w->entities[i];
        if (worldEntity.type == WE_ITEM && entitiesIntersect(p->entity, *worldEntity.entity)) {
            // checking if player already has those items
            for (int j = 0; j < 9; j++) {
                hotbarItem_t *slot = &p->hotbar.slots[j];
                if (slot->type == worldEntity.itemType && slot->count < ITEM_PROPERTIES[slot->type].maxStackSize) {
                    slot->count++;
                    player_printHotbar(p);
                    world_removeItemEntity(w, i);
                    return;
                }
            }

            // if player does not already have matching item, assign to first empty slot
            for (int j = 0; j < 9; j++) {
                hotbarItem_t *slot = &p->hotbar.slots[j];
                if (slot->type == NOTHING) {
                    slot->type = worldEntity.itemType;
                    slot->count = 1;
                    player_printHotbar(p);
                    world_removeItemEntity(w, i);
                    return;
                }
            }
        }
    }
}
