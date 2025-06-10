#pragma once

#include "block.h"

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
    /// Any other flags (yet to be implemented)
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

/// a mapping from block item types to the block types
static const block_t BLOCK_TO_ITEM[] = {
    [BL_DIRT]  = ITEM_DIRT,
    [BL_GRASS] = ITEM_GRASS,
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
