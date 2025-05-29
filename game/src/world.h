#pragma once

#include <glad/gl.h>
#include <stddef.h>
#include "chunk.h"

#define MAX_CHUNKS 16

/**
 * @brief A struct that holds data about the world.
 */
typedef struct {
    /// The array of loaded chunks.
    chunk_t loadedChunks[MAX_CHUNKS];
    /// The current number of chunks/next free space in the array.
    size_t chunkN;
} world_t;

/**
 * @brief Initialises a world struct.
 * @param w A pointer to a world
 */
void world_init(world_t *w);

/**
 * @brief Adds a chunk to the world (maybe).
 * @param w A pointer to a world
 * @param chunk The chunk to add
 * @return Whether the function was successful
 */
bool world_addChunk(world_t *w, chunk_t chunk);

/**
 * @brief Draws the world.
 * @param w A pointer to a world
 * @param modelLocation The model matrix location in the shader program
 */
void world_draw(const world_t *w, int modelLocation);
