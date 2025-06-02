#pragma once

#include <glad/gl.h>
#include <stdbool.h>
#include "block.h"

#define CHUNK_SIZE 16
#define CHUNK_SIZE_CUBED 4096

/**
 * @brief A struct containing data about a chunk.
 */
typedef struct {
    /// Chunk coordinates.
    int cx, cy, cz;
    /// The array of blocks in the chunk.
    block_t blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    /// The 3D array of blocks as a 3D texture
    GLuint blockTexture;
    /// The VBO that holds the mesh.
    GLuint vbo;
    /// The VAO that is used for drawing.
    GLuint vao;
    /// Number of vertices in the current mesh
    int meshVertices;
    /// Holds whether the mesh needs to be regenerated.
    bool tainted;
} chunk_t;

/**
 * @brief A function to initialise a chunk by filling it with a certain block.
 * @param c A pointer to a chunk
 * @param cx The chunk x coordinate
 * @param cy The chunk y coordinate
 * @param cz The chunk z coordinate
 * @param block The type of block
 * @note The chunk object should be empty/uninitialised.
 */
void chunk_create(chunk_t *c, int cx, int cy, int cz, block_t block);

/**
 * @brief A function to generate a chunk
 * @param c A pointer to a chunk
 * @param cx The chunk x coordinate
 * @param cy The chunk y coordinate
 * @param cz The chunk z coordinate
 * @note The chunk object should be empty/uninitialised.
 */
void chunk_generate(chunk_t *c, int cx, int cy, int cz);

/**
 * @brief Draws a chunk.
 * @param c A pointer to a chunk
 * @param modelLocation The location of the model matrix in the shader program
 */
void chunk_draw(const chunk_t *c, int modelLocation);

void chunk_free(const chunk_t *c);