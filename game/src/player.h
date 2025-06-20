#ifndef PLAYER_H
#define PLAYER_H

#include <cglm/cglm.h>
#include "camera.h"
#include "entity.h"
#include "item.h"

#define BLOCK_COOLDOWN_TIME 0.5
#define HOTBAR_SLOTS 9

/**
 * @brief A struct containing hotbar data
 */
typedef struct {
    /// each of the slots in the hotbar
    hotbarItem_t slots[HOTBAR_SLOTS];
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
    /// The block the player is currently mining
    vec3 miningBlockPos;
    /// How long the player has been mining for
    double currMiningTime;
} player_t;

/**
 * @brief Initialises a player object.
 * @param w A pointer to a world
 * @param p A pointer to a player
 */
void player_init(world_t *w, player_t *p);

/**
 * @brief Moves a camera to the player and sets orientation of the player.
 * @param p A pointer to a player
 * @param camera A pointer to a camera
 */
void player_attachCamera(player_t *p, camera_t *camera);

/**
 * @brief Mines the block a player is looking at
 * @param p A pointer to a player
 * @param w A pointer to a world
 * @param dt The time since the last frame
 */
void player_mineBlock(player_t *p, world_t *w, double dt);

/**
 * @brief Places a block where a player is looking at, if the player is off cooldown.
 * @param p A pointer to a player
 * @param w A pointer to a world
 */
void player_placeBlock(player_t *p, world_t *w);

/**
 * @brief Prints a player's hotbar to the terminal
 * @param p A pointer to a player
 */
void player_printHotbar(const player_t *p);

/**
 * @brief Checks if a player should pick up any items
 * @param p A pointer to a player
 * @param w A pointer to a world
 */
void player_pickUpItemsCheck(player_t *p, world_t *w);

#endif
