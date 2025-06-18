#ifndef WORLD_H
#define WORLD_H

#include <cglm/cglm.h>
#include <glad/gl.h>
#include "camera.h"
#include "chunk.h"
#include "item.h"
#include "noise.h"
#include "player.h"

#define MAX_CHUNKS 256
#define MAX_CHUNK_LOADERS 8
#define C_T 8
#define LOG_C_T 3

#define CHUNK_LOAD_RADIUS 7

#define FOG_START 16.f * (CHUNK_LOAD_RADIUS - 2)
#define FOG_END 16.f * (CHUNK_LOAD_RADIUS - 1)

#define GRAVITY_ACCELERATION (-10.f)

#define MAX_NUM_ENTITIES 64
#define MAX_NUM_PLAYERS 4

typedef struct entity_t entity_t;

typedef enum {
    WE_NONE,
    WE_PLAYER,
    WE_ITEM,
    // WE_MOB,   (Not implemented yet)
} worldEntity_e;

typedef struct {
    /// What kind of entity it is
    worldEntity_e type;
    /// The actual entity_t entity
    entity_t *entity;
    /// This is only checked if 'type' is WE_ITEM
    item_e itemType;
    /// Stores whether the entity needs to be freed
    bool needsFreeing;
    /// Entity VAO and VBO, currently only used for 'item' entities
    GLuint vao;
    GLuint vbo;
} worldEntity_t;

/**
 * @brief A struct that holds data about the world.
 */
typedef struct world_t {
    /// The array of chunk loaders present
    struct {
        bool active;
        int x, y, z;
    } chunkLoaders[MAX_CHUNK_LOADERS];
    /// The hash table used keeping track of chunks
    struct _s_cluster *clusterTable;
    GLuint highlightVao;
    GLuint highlightVbo;
    mat4 highlightModel;
    bool highlightFound;
    int numEntities;
    worldEntity_t entities[MAX_NUM_ENTITIES];
    int oldestItem;
    int numPlayers;
    /// Stores pointers to all current players in the world
    worldEntity_t *players[MAX_NUM_PLAYERS];
    uint64_t seed;
    rng_t generalRng;
    rng_t worldRng;
    noise_t noise;
} world_t;

/**
 * @brief Contains data about what stage of loading the chunk is
 */
typedef enum {
    LL_INIT = 0,
    LL_PARTIAL = 1,
    LL_TOTAL = 2,
} chunkLoadLevel_e;

/**
 * @brief Contains data about the reload style of a chunk
 */
typedef enum {
    REL_TOP_RELOAD = 0,
    REL_CHILD = 2,
    REL_TOP_UNLOAD = 3,
    REL_TOMBSTONE = 3
} reloadData_e;


/**
 * @brief Value stored in hashmap entry array
 */
typedef struct chunkValue_t {
    /// The pointer to a heap allocated chunk
    chunk_t *chunk;
    /// The current level of loading the chunk is in
    chunkLoadLevel_e ll;

    struct {
        reloadData_e reload;
        size_t nChildren;
        struct chunkValue_t *children[32];
    } loadData;

} chunkValue_t;

/**
 * @brief Initialises a world struct.
 * @param w A pointer to a world
 * @param seed The world seed
 */
void world_init(world_t *w, uint64_t seed);

/**
 * @brief Draws the world.
 * @param w A pointer to a world
 * @param modelLocation The model matrix location in the shader program
 * @param cam A pointer to the camera from which to render from
 * @param projection The current projection matrix
 */
void world_draw(const world_t *w, int modelLocation, camera_t *cam, mat4 projection);

/**
 * @brief Frees the world.
 * @param w A pointer to a world
 */
void world_free(world_t *w);

/**
* @brief Gets a fully loaded chunk from chunk coordinates.
* @param w A pointer to a world
* @param cx Target chunk's x coordinate
* @param cy Target chunk's y coordinate
* @param cz Target chunk's z coordinate
* @return A pointer to the chunk. Returns NULL if chunk isn't fully loaded.
*/
chunk_t *world_getFullyLoadedChunk(world_t *w, const int cx, const int cy, const int cz);

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
bool world_getBlock(world_t *w, const vec3 pos, blockData_t *bd);

/**
 * @brief Gets all adjacent blocks to a block at a specific position
 * @param w a pointer to the worlda
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
 * @param raycastDistance the maximum distance to check with raycasting
 * @return whether the raycast was successful - found = False mean no block was found, otherwise found = true
 *         and block = found block
 */
raycast_t world_raycast(world_t *w, vec3 startPosition, vec3 viewDirection, float raycastDistance);

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
bool world_save(const world_t *w, const char *dir);

/**
* @brief Highlights the face the camera is looking at
* @param w A pointer to a world
* @param camera A pointer to a camera
*/
void world_highlightFace(world_t *w, camera_t *camera);

/**
* @brief Draws the highlighted face
* @param w A pointer to a world
* @param modelLocation The model location
*/
void world_drawHighlight(const world_t *w, int modelLocation);

/**
 * @brief Updates all entities and their positions/velcities
 * @param w A pointer to a world
 * @param dt the time since the function was last run
 */
void world_processAllEntities(world_t *w, double dt);

/**
 * @brief Adds an entity to the world
 * @param w A pointer to a world
 * @param we A world entity
 */
void world_addEntity(world_t *w, worldEntity_t we);

/**
 * @brief Removes an entity from the world
 * @param w A pointer to a world
 * @param entityIndex The index of the entity in the world entity array
 */
void world_removeItemEntity(world_t *w, int entityIndex);

void world_drawAllEntities(const world_t *w, int modelLocation);

/**
 * @brief Gets the type of block at a chosen position
 * @param w a pointer to the world
 * @param position the position to get a block at
 * @return The type of the block
 */
block_t getBlockType(world_t *w, vec3 position);

/**
 * @brief Loads a chunk.
 * @param w A pointer to a world
 * @param cx Chunk x coordinate
 * @param cy Chunk y coordinate
 * @param cz Chunk z coordinate
 * @param ll The load level to load to if the chunk doesn't exist
 * @param r The reload style of the chunk
 */
chunkValue_t *world_loadChunk(world_t *w,
                     const int cx,
                     const int cy,
                     const int cz,
                     const chunkLoadLevel_e ll,
                     const reloadData_e r);

#endif