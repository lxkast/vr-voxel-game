#ifndef CHUNK_H
#define CHUNK_H

#include <glad/gl.h>
#include <stdbool.h>
#include <stdio.h>

#include "block.h"
#include "queue.h"
#include "noise.h"
#include "spscqueue.h"
#include "vertices.h"

#define CHUNK_SIZE 16
#define CHUNK_SIZE_CUBED 4096

typedef struct world_t world_t;

/**
 * @brief An enum containing biome information
 */
typedef enum {
    BIO_NIL,
    BIO_FOREST,
    BIO_PLAINS,
    BIO_DESERT,
    BIO_TUNDRA,
    BIO_CAVE,
    BIO_JUNGLE,
} biome_e;

/**
 * @brief A struct containing data about a chunk.
 */
typedef struct {
    /// Chunk coordinates.
    int cx, cy, cz;
    /// The array of blocks in the chunk.
    block_t blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    /// The array of light levels in the chunk.
    unsigned char lightMap[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    /// The queue of light values used for adding lights to the lightMap
    lightQueue_t lightTorchInsertionQueue;
    lightQueue_t lightSunInsertionQueue;
    /// The queue of PREVIOUS light values for deleting lights from the lightMap
    lightQueue_t lightTorchDeletionQueue;
    lightQueue_t lightSunDeletionQueue;
    /// The VBO that holds the mesh.
    GLuint vbo;
    /// The VAO that is used for drawing.
    GLuint vao;
    /// Number of vertices in the current mesh
    int meshVertices;
    /// Holds whether the mesh needs to be regenerated.
    bool tainted;

    vertex_t *vertices;
    bool verticesValid;

    /// A rng for use in terrain generation
    rng_t rng;
    /// A noise object
    noise_t noise;
    /// The biome of the chunk
    biome_e biome;
} chunk_t;

/**
 * @brief Initialises a chunk
 * @param c A pointer to a chunk
 * @param rng An rng
 * @param noise A noise object
 * @param cx The chunk x coordinate
 * @param cy The chunk y coordinate
 * @param cz The chunk z coordinate
 */
void chunk_init(chunk_t *c, rng_t rng, noise_t noise, int cx, int cy, int cz);

/**
 * @brief Fills a chunk with a certain block
 * @param c A pointer to a chunk
 * @param block The type of block
 * @note The chunk object should be empty/uninitialised.
 */
void chunk_fill(chunk_t *c, block_t block);

/**
* @brief Loads chunk data from a file
* @param c A pointer to a chunk
* @param fp A file pointer
*/
void chunk_createDeserialise(chunk_t *c, FILE *fp);

/**
* @brief Queues sunlight values in transparent blocks at the top of the chunk to be propagated downwards
* @param c A pointer to a chunk
*/
void chunk_initSun(chunk_t *c);

/**
 * @brief A function to generate a chunk
 * @param c A pointer to a chunk
 * @note The chunk object should be empty/uninitialised.
 */
void chunk_generate(chunk_t *c);

/**
 * @brief Remeshes the chunk if necessary
 * @param c A pointer to a chunk
 * @param w A pointer to a world
 */
void chunk_checkMesh(chunk_t *c, world_t *w);

/**
 * @brief Checks if a mesh is invalid, and if so generates one.
 * @param c A pointer to a chunk
 * @param w A pointer to a world
 */
void chunk_checkGenMesh(chunk_t *c, world_t *w);

/**
 * @brief Draws a chunk.
 * @param c A pointer to a chunk
 * @param modelLocation The location of the model matrix in the shader program
 */
void chunk_draw(const chunk_t *c, int modelLocation);

/**
* @brief A function for freeing a chunk
* @param c A pointer to a chunk
* @param freeQueue A queue of VBOs to free
*/
void chunk_free(chunk_t *c, spscRing_t *freeQueue);

void main_thread_free(spscRing_t *freeQueue);

/**
* @brief A function to serialise a chunk to a file
* @param c A pointer to a chunk
* @param fp A file pointer
*/
void chunk_serialise(const chunk_t *c, FILE *fp);

#endif