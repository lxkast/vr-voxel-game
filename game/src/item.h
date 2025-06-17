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
    ITEM_LOG,
    ITEM_LEAF,
    ITEM_GLOWSTONE,
    ITEM_SNOW,
    ITEM_JUNGLE_GRASS,
    ITEM_MUD,
    ITEM_CACTUS,
    ITEM_JUNGLE_LOG,
    ITEM_JUNGLE_LEAF
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
    /// A relative file path to the audio file to be play when breaking a block
    char *breakSound;
    /// Any other flags (yet to be implemented)
} itemProperties_t;

/*
 * All audio files are free to use under copyright laws.
 * See the pixabay content license here:
 * https://pixabay.com/service/license-summary/
 *
 * Below I have included links to all audio files used in the project.
 * Please note the majority have been edited to some degree (either by
 * changing the volume or cutting them down in size).
 *
 * All files available and free to use as of 17/06/2025.
 *
 * Glass Breaking: https://pixabay.com/sound-effects/breaking-glass-83809/
 * Dirt/Sand/Grass... Breaking: https://pixabay.com/sound-effects/footsteps-dirt-gravel-6823/
 * Leaf Breaking: https://pixabay.com/sound-effects/rustling-bushes-dried-leaves-5-230204/
 * Stone Breaking: https://pixabay.com/sound-effects/rock-destroy-6409/
 * Log Breaking: https://pixabay.com/sound-effects/log-split-88986/
 *
 */

/// a mapping from the item type to its properties.
static const itemProperties_t ITEM_PROPERTIES[] = {
    [NOTHING]    = {"Empty",      false, false, 0, -1, ""},
    [ITEM_DIRT]  = {"Dirt Block",       true,  false,  64, -1,"../../src/audio/sand_dirt_grass_destroy.mp3"},
    [ITEM_GRASS] = {"Grass Block",      true,  false,  64,-1, "../../src/audio/sand_dirt_grass_destroy.mp3"},
    [ITEM_STONE] = {"Stone Block",       true,  false,  64,-1, "../../src/audio/stone_destroy.mp3"},
    [ITEM_SWORD] = {"Sword",      false, true, 1,200, ""},
    [ITEM_SHOVEL] = {"Shovel",      false, true, 1,200, ""},
    [ITEM_PICKAXE] = {"Pickaxe",      false, true, 1,200, ""},
    [ITEM_LOG] = {"Log",       true,  false,  64,-1, "../../src/audio/log_destroy.mp3"},
    [ITEM_LEAF] = {"Leaf block",       true,  false,  64,-1, "../../src/audio/leaf_destroy.mp3"},
    [ITEM_GLOWSTONE] = { "Glowstone", true, false, 64, -1, "../../src/audio/glass_breaking.mp3"},
    [ITEM_SNOW] = { "Snow", true, false, 64, -1, "../../src/audio/sand_dirt_grass_destroy.mp3"},
    [ITEM_JUNGLE_GRASS] = { "Jungle grass", true, false, 64, -1, "../../src/audio/sand_dirt_grass_destroy.mp3"},
    [ITEM_MUD] = { "Mud", true, false, 64, -1, "../../src/audio/sand_dirt_grass_destroy.mp3"},
    [ITEM_CACTUS] = { "Cactus", true, false, 64, -1, "../../src/audio/log_destroy.mp3"},
    [ITEM_JUNGLE_LOG] = { "Jungle log", true, false, 64, -1, "../../src/audio/log_destroy.mp3"},
    [ITEM_JUNGLE_LEAF] = { "Jungle leaf", true, false, 64, -1, "../../src/audio/leaf_destroy.mp3"},
};

/// a mapping from block item types to the block types
static const block_t ITEM_TO_BLOCK[] = {
    [ITEM_DIRT]  = BL_DIRT,
    [ITEM_GRASS] = BL_GRASS,
    [ITEM_LOG] = BL_LOG,
    [ITEM_LEAF] = BL_LEAF,
    [ITEM_STONE] = BL_STONE,
    [ITEM_GLOWSTONE] = BL_GLOWSTONE,
    [ITEM_SNOW] = BL_SNOW,
    [ITEM_JUNGLE_GRASS] = BL_JUNGLE_GRASS,
    [ITEM_MUD] = BL_MUD,
    [ITEM_CACTUS] = BL_CACTUS,
    [ITEM_JUNGLE_LOG] = BL_JUNGLE_LOG,
    [ITEM_JUNGLE_LEAF] = BL_JUNGLE_LEAF,
};

/// a mapping from block item types to the block types
static const block_t BLOCK_TO_ITEM[] = {
    [BL_DIRT]  = ITEM_DIRT,
    [BL_GRASS] = ITEM_GRASS,
    [BL_STONE] = ITEM_STONE,
    [BL_LOG] = ITEM_LOG,
    [BL_LEAF] = ITEM_LEAF,
    [BL_GLOWSTONE] = ITEM_GLOWSTONE,
    [BL_SNOW] = ITEM_SNOW,
    [BL_JUNGLE_GRASS] = ITEM_JUNGLE_GRASS,
    [BL_MUD] = ITEM_MUD,
    [BL_CACTUS] = ITEM_CACTUS,
    [BL_JUNGLE_LOG] = ITEM_JUNGLE_LOG,
    [BL_JUNGLE_LEAF] = ITEM_JUNGLE_LEAF,
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
