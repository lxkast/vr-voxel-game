#pragma once

#include <cglm/cglm.h>
#include "camera.h"
#include "entity.h"

#define BLOCK_COOLDOWN_TIME 0.5

/// The different types of items that the game can have
typedef enum {
    NOTHING,
    ITEM_DIRT,
    ITEM_GRASS,
    ITEM_STONE,
    ITEM_SWORD,
    ITEM_SHOVEL,
    ITEM_PICKAXE,
} item_e;

typedef struct {
    /// the name displayed by the UI when interacting with the item
    char *displayName;
    /// whether the item is placeable
    bool isPlaceable;
    /// if the item is a tool
    bool isTool;
    /// the maximum number of the item in a stack
    int maxStackSize;
    /// the durability of the item (if it is a tool)
    /// -1 means N/A - otherwise decreases by 1 each time it is used
    int durability;
    /// Any other flags (yet to be implemnted)
} itemProperties_t;

/// a mapping from the item type to its properties.
static const itemProperties_t ITEM_PROPERTIES[] = {
    [NOTHING]    = {"Empty",      false, false, 0, -1},
    [ITEM_DIRT]  = {"Dirt Block",       true,  false,  64, -1},
    [ITEM_GRASS] = {"Grass Block",      true,  false,  64,-1},
    [ITEM_STONE] = {"Stone Block",       true,  false,  64,-1},
    [ITEM_SWORD] = {"Sword",      false, true, 1,200},
    [ITEM_SHOVEL] = {"Shovel",      false, true, 1,200},
    [ITEM_PICKAXE] = {"Pickaxe",      false, true, 1,200},
};

/// a mapping from block item types to the block types
static const block_t ITEM_TO_BLOCK[] = {
    [ITEM_DIRT]  = BL_DIRT,
    [ITEM_GRASS] = BL_GRASS,
};

typedef struct {
    /// the type of the item
    item_e type;
    /// how many of the item is in the hotbar slot  (count <= max stack size for that item)
    char count;
    /// could add more members in the future, such as durability etc
    /// this will have to be fleshed out more later
    /// maybe members like "isBlock" or "isWeapon"
} hotbarItem_t;

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