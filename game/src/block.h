#ifndef BLOCK_H_
#define BLOCK_H_

#include <cglm/cglm.h>

#define ENABLE_AUDIO

typedef enum {
    BL_AIR,
    BL_GRASS,
    BL_DIRT,
    BL_SAND,
    BL_STONE,
    BL_LOG,
    BL_LEAF,
    BL_GLOWSTONE,
    BL_SNOW,
    BL_JUNGLE_GRASS,
    BL_MUD,
    BL_CACTUS,
    BL_JUNGLE_LOG,
    BL_JUNGLE_LEAF
} block_t;

static const float TIME_TO_MINE_BLOCK[] = {
    [BL_AIR]          = 0.f,
    [BL_GRASS]        = 0.5f,
    [BL_DIRT]         = 0.5f,
    [BL_SAND]         = 0.4f,
    [BL_STONE]        = 3.f,
    [BL_LOG]          = 1.f,
    [BL_LEAF]         = 0.1f,
    [BL_GLOWSTONE]    = 0.3f,
    [BL_SNOW]         = 0.5f,
    [BL_JUNGLE_GRASS] = 0.5f,
    [BL_MUD]          = 0.75f,
    [BL_CACTUS]       = 0.4f,
    [BL_JUNGLE_LOG]   = 1.f,
    [BL_JUNGLE_LEAF]  = 0.1f,
};

#define BL_TRANSPARENT(x) (x == BL_AIR || x == BL_LEAF || x == BL_JUNGLE_LEAF)

/**
 * @brief Struct that holds data about a single block.
 */
typedef struct {
    /// The x, y and z coordinates of the block
    int x, y, z;
    /// The type of the block
    block_t type;
} blockData_t;

/**
 * @brief Enum for each face of a block from a raycast
 */
typedef enum {
    POS_X_FACE,
    NEG_X_FACE,
    POS_Y_FACE,
    NEG_Y_FACE,
    POS_Z_FACE,
    NEG_Z_FACE,
} raycastFace_e;

/**
 * @brief Struct that holds data about the result of a raycast
 */
typedef struct {
    /// The position of the block found by the raycast (if it finds one)
    vec3 blockPosition;
    /// The face of the block the player is looking at
    raycastFace_e face;
    /// If a valid block was found
    bool found;
} raycast_t;

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

#ifdef ENABLE_AUDIO
static const char *BLOCK_TO_AUDIO[] = {
    [BL_DIRT]  = "../../src/audio/sand_dirt_grass_destroy.mp3",
    [BL_GRASS] = "../../src/audio/sand_dirt_grass_destroy.mp3",
    [BL_STONE] = "../../src/audio/stone_destroy.mp3",
    [BL_LOG] = "../../src/audio/log_destroy.mp3",
    [BL_LEAF] = "../../src/audio/leaf_destroy.mp3",
    [BL_GLOWSTONE] = "../../src/audio/glass_breaking.mp3",
    [BL_SNOW] = "../../src/audio/sand_dirt_grass_destroy.mp3",
    [BL_JUNGLE_GRASS] = "../../src/audio/sand_dirt_grass_destroy.mp3",
    [BL_MUD] = "../../src/audio/sand_dirt_grass_destroy.mp3",
    [BL_CACTUS] = "../../src/audio/log_destroy.mp3",
    [BL_JUNGLE_LOG] = "../../src/audio/log_destroy.mp3",
    [BL_JUNGLE_LEAF] = "../../src/audio/leaf_destroy.mp3",
};
#endif

#endif