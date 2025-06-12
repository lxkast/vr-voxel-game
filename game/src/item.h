#pragma once

#include "block.h"

/// The different types of items that the game can have
typedef enum {
    NOTHING,
    ITEM_DIRT,
    ITEM_GRASS,
    ITEM_SAND,
    ITEM_STONE,
    ITEM_SWORD,
    ITEM_SHOVEL,
    ITEM_PICKAXE,
    ITEM_AXE,
    ITEM_LOG,
    ITEM_LEAF,
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
    /// the durability of the item (if it is a tool).
    /// -1 means N/A - otherwise decreases by 1 each time it is used
    int durability;
    /// if the item is a tool, the speedup it gives when used on the correct block
    double miningBoost;
    /// Any other flags (yet to be implemented)
} itemProperties_t;

/// a mapping from the item type to its properties.
static const itemProperties_t ITEM_PROPERTIES[] = {
    [NOTHING]      = {"Empty",      false, false, 0, -1, 1.},
    [ITEM_DIRT]    = {"Dirt Block",       true,  false,  64, -1, 1.},
    [ITEM_GRASS]   = {"Grass Block",      true,  false,  64,-1,1.},
    [ITEM_SAND]    = {"Sand Block",       true,  false,  64,-1,1.},
    [ITEM_STONE]   = {"Stone Block",       true,  false,  64,-1,1.},
    [ITEM_SWORD]   = {"Sword",      false, true, 1,200,1.},
    [ITEM_SHOVEL]  = {"Shovel",      false, true, 1,200,2.},
    [ITEM_PICKAXE] = {"Pickaxe",      false, true, 1,200,3.},
    [ITEM_AXE]     = {"Axe",      false, true, 1,200,2.5},
    [ITEM_LOG]     = {"Log",       true,  false,  64,-1, 1.},
    [ITEM_LEAF]    = {"Leaf block",       true,  false,  64,-1, 1.},
};

/// a mapping from block item types to the block types
static const block_t ITEM_TO_BLOCK[] = {
    [ITEM_DIRT]  = BL_DIRT,
    [ITEM_GRASS] = BL_GRASS,
    [ITEM_SAND] = BL_SAND,
    [ITEM_LOG] = BL_LOG,
    [ITEM_LEAF] = BL_LEAF,
    [ITEM_STONE] = BL_STONE,
};

/// a mapping from block item types to the block types
static const block_t BLOCK_TO_ITEM[] = {
    [BL_DIRT]  = ITEM_DIRT,
    [BL_GRASS] = ITEM_GRASS,
    [BL_SAND]  = ITEM_SAND,
    [BL_STONE] = ITEM_STONE,
    [BL_LOG]   = ITEM_LOG,
    [BL_LEAF]  = ITEM_LEAF,
};

static const item_e BLOCK_TO_TOOL[] = {
    [BL_DIRT]  = ITEM_SHOVEL,
    [BL_GRASS] = ITEM_SHOVEL,
    [BL_SAND]  = ITEM_SHOVEL,
    [BL_STONE] = ITEM_PICKAXE,
    [BL_LOG]   = ITEM_AXE,
    [BL_LEAF]  = NOTHING,
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
