#pragma once

#include <cglm/cglm.h>
#include <glad/gl.h>
#include <stddef.h>
#include "chunk.h"
#include "uthash.h"
#include "camera.h"

#define MAX_CHUNKS 256
#define MAX_CHUNK_LOADERS 8
#define C_T 8
#define LOG_C_T 3

#define CHUNK_LOAD_RADIUS 4

#define FOG_START 16.f * (CHUNK_LOAD_RADIUS - 2)
#define FOG_END 16.f * (CHUNK_LOAD_RADIUS - 1)

/**
 * @brief A struct that holds data about the world.
 */
typedef struct {
    /// The array of chunk loaders present
    struct {
        bool active;
        int x, y, z;
    } chunkLoaders[MAX_CHUNK_LOADERS];
    /// The hash table used keeping track of chunks
    struct _s_cluster *clusterTable;
} world_t;

/**
 * @brief Struct that holds data about a single block.
 */
typedef struct {
    int x, y, z;
    block_t type;
} blockData_t;

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
    vec3 blockPosition;
    raycastFace_e face;
    bool found;
} raycast_t;

/**
 * @brief Initialises a world struct.
 * @param w A pointer to a world
 * @param program A shader program for setting effects
 */
void world_init(world_t *w, GLuint program);

/**
 * @brief Draws the world.
 * @param w A pointer to a world
 * @param modelLocation The model matrix location in the shader program
 */
void world_draw(const world_t *w, int modelLocation, camera_t *cam);

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
 * @brief Gets a block (if possible) at by the block position.
 * @param w A pointer to a world
 * @param x Block x
 * @param y Block y
 * @param z Block z
 * @param bd A block data struct to allocate to
 * @return Whether the operation was successful
 */
bool world_getBlocki(world_t *w, int x, int y, int z, blockData_t *bd);

/**
 * @brief Gets a block (if possible) at a position.
 * @param w A pointer to a world
 * @param pos The position you want to check for a block at
 * @param bd A block data struct to allocate to
 * @return Whether the operation was successful
 */
bool world_getBlock(world_t *w, vec3 pos, blockData_t *bd);

/**
 * @brief Gets all adjacent blocks to a block at a specific position
 * @param w a pointer to the world
 * @param position the position of the block
 * @param buf the array where the blocks are stored
 */
void world_getAdjacentBlocks(world_t *w, vec3 position, blockData_t *buf);

/**
 * @brief gets all blocks within a cuboid defined by two opposite corners
 * @param w a pointer to the world
 * @param bottomLeft bottom left corner of the cuboid
 * @param topRight top right corner of the cuboid
 * @param buf the array where the blocks are stored
 */
void world_getBlocksInRange(world_t *w, vec3 bottomLeft, const vec3 topRight, blockData_t *buf);

/**
 * @brief Performs raycasting from a point at a specific angle
 * @param w a pointer to the world
 * @param startPosition the position to start the raycast from
 * @param viewDirection the direction to raycast along
 * @return whether the raycast was successful - found = False mean no block was found, otherwise found = true
 *         and block = found block
 */
raycast_t world_raycast(world_t *w, vec3 startPosition, vec3 viewDirection);

/**
 * @brief Tries to remove a block at a position.
 * @param w A pointer to the world
 * @param x Block x
 * @param y Block y
 * @param z Block z
 * @return Whether the operation was successful
 */
bool world_removeBlock(world_t *w, int x, int y, int z);

/**
 * @brief Tries to place a block at a position.
 * @param w A pointer to the world
 * @param x Block x
 * @param y Block y
 * @param z Block z
 * @param block The type of block to try and place
 * @return Whether the operation was successful
 */
bool world_placeBlock(world_t *w, int x, int y, int z, block_t block);

/**
* @brief Saves the currently loaded world
* @param w A pointer to a world
* @param dir The name of the directory to save the file in
*/
bool world_save(world_t *w, const char *dir);
