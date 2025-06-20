#ifndef BLOCK_H_
#define BLOCK_H_

#include <cglm/cglm.h>

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
    /// The position of the block
    vec3 blockPosition;
    /// The face of the block the player is looking at
    raycastFace_e face;
    /// If a valid block was found
    bool found;
} raycast_t;

#endif