#pragma once

#include <cglm/cglm.h>
#include "camera.h"
#include "entity.h"
#include "item.h"
#include "world.h"

#define BLOCK_COOLDOWN_TIME 0.5

typedef struct {
    /// each of the slots in the hotbar
    hotbarItem_t slots[9];
    /// the slot that is currently active
    hotbarItem_t *currentSlot;
    /// the index of the current slot
    char currentSlotIndex;
} hotbar_t;

/**
 * @brief A struct containing player data.
 */
typedef struct {
    /// The player's entity.
    entity_t entity;
    /// The direction in which the player is looking
    vec3 lookVector;
    /// The displacement of the camera from the player
    vec3 cameraOffset;
    /// The cooldown before a block can be placed again
    double blockCooldown;
    /// The player's hotbar
    hotbar_t hotbar;
} player_t;

/**
 * @brief Initialises a player object.
 * @param p A pointer to a player
 */
void player_init(player_t *p);

/**
 * @brief Moves a camera to the player and sets orientation of the player.
 * @param p A pointer to a player
 * @param camera A pointer to a camera
 */
void player_attachCamera(player_t *p, camera_t *camera);

void player_removeBlock(player_t *p, world_t *w);

void player_placeBlock(player_t *p, world_t *w);

void player_printHotbar(const player_t *p);

void player_pickUpItemsCheck(player_t *p, world_t *w);