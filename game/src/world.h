#pragma once

#include <glad/gl.h>
#include <stddef.h>
#include "chunk.h"
#include "uthash.h"
#include <cglm/cglm.h>

#define MAX_CHUNKS 256
#define MAX_CHUNK_LOADERS 8
#define C_T 8
#define LOG_C_T 3


/**
 * @brief A struct that holds data about the world.
 */
typedef struct {
    /// The array of chunk loaders present
    struct { bool active; int x, y, z; } chunkLoaders[MAX_CHUNK_LOADERS];
    /// The hash table used keeping track of chunks
    struct _s_cluster *clusterTable;
} world_t;

/**
 * @brief Struct that holds data about a single block.
 */
typedef struct {
    int x, y, z;
    block_t type;
} block_data_t;

/**
 * @brief Initialises a world struct.
 * @param w A pointer to a world
 */
void world_init(world_t *w);

/**
 * @brief Draws the world.
 * @param w A pointer to a world
 * @param modelLocation The model matrix location in the shader program
 */
void world_draw(const world_t *w, int modelLocation);

/**
 * @brief Frees the world.
 * @param w A pointer to a world
 */
void world_free(world_t *w);

/**
 * @brief Tries to assign a new chunk loader id.
 * @param w A pointer to a world
 * @param id A pointer to an unsigned int that the new id will be written to
 * @return Whether an id was successfully assigned
 */
bool world_genChunkLoader(world_t *w, unsigned int *id);

/**
 * @brief Updates the position of a specific chunk loader.
 * @param w A pointer to a world
 * @param id The id of the chunk loader
 * @param pos The new position
 * @note Silently fails if the id is not valid
 */
void world_updateChunkLoader(world_t *w, unsigned int id, const float pos[3]);

/**
 * @brief Deletes a chunk loader from an id.
 * @param w A pointer to a world
 * @param id The id of the chunk loader
 */
void world_delChunkLoader(world_t *w, unsigned int id);

/**
 * @brief Considers all chunk loaders and loads the correct chunks
 * @param w A pointer to a world
 */
void world_doChunkLoading(world_t *w);

/**
 * @brief Gets a block (if possible) at a position.
 * @param w A pointer to a world
 * @param pos The position you want to check for a block at
 * @param bd A block data struct to allocate to
 * @return Whether the operation was successful
 */
bool world_getBlock(world_t *w, vec3 pos, block_data_t *bd);

/**
 * @brief Gets all adjacent blocks to a block at a specific position
 * @param w a pointer to the world
 * @param position the position of the block
 * @param buf the array where the blocks are stored
 */
void world_getAdjacentBlocks(world_t *w, vec3 position, block_data_t *buf);

/**
 * @brief gets all blocks within a cuboid defined by two opposite corners
 * @param w a pointer to the world
 * @param bottomLeft bottom left corner of the cuboid
 * @param topRight top right corner of the cuboid
 * @param buf the array where the blocks are stored
 */
void world_getBlocksInRange(world_t *w, vec3 bottomLeft, vec3 topRight, block_data_t *buf);