#include <logging.h>
#include <cglm/cglm.h>
#include <stdlib.h>
#include <string.h>
#include "chunk.h"
#include "lighting.h"
#include "vertices.h"
#include "GLFW/glfw3.h"

/**
 * @brief Checks if a block's face neighbours air or the chunk edge
 * @param w A pointer to a world
 * @param c A pointer to a chunk
 * @param blockPos The position of the block to check
 * @param dir The face of the block to check
 * @return If the face is visible
 */
static bool faceIsVisible(world_t *w, const chunk_t *c, ivec3 blockPos, const direction_e dir) {
    ivec3 dirVec;
    memcpy(&dirVec, &directions[dir], sizeof(ivec3));
    ivec3 neighbourPos;
    glm_ivec3_add(blockPos, dirVec, neighbourPos);
    ivec3 chunkOffset = { 0, 0, 0 };
    for (int i = 0; i < 3; i++) {
        if (neighbourPos[i] < 0) {
            neighbourPos[i] = CHUNK_SIZE - 1;
            chunkOffset[i] = -1;
        } else if (neighbourPos[i] >= CHUNK_SIZE) {
            neighbourPos[i] = 0;
            chunkOffset[i] = 1;
        }
    }
    if (chunkOffset[0] == 0 && chunkOffset[1] == 0 && chunkOffset[2] == 0) {
        block_t type = c->blocks[neighbourPos[0]][neighbourPos[1]][neighbourPos[2]];
        return BL_TRANSPARENT(type);
    } else {
        ivec3 cPos = { c->cx, c->cy, c->cz };
        glm_ivec3_add(cPos, chunkOffset, cPos);
        chunk_t *nChunk = world_getFullyLoadedChunk(w, cPos[0], cPos[1], cPos[2]);
        if (!nChunk) {
            return true;
        }
        block_t type = nChunk->blocks[neighbourPos[0]][neighbourPos[1]][neighbourPos[2]];
        return BL_TRANSPARENT(type);
    }
}

/**
 * @brief Writes vertices of a face specified by buf
 * @param w A pointer to a world
 * @param c A pointer to a chunk
 * @param buf A buffer of vertices
 * @param blockPos The position of the block
 * @param dir The face of the block
 * @param width The width of the block
 * @param height The heigh of the block
 * @param type The type of block
 * @return Pointer to next free position in buffer
 */
static vertex_t *writeFace(world_t *w,
                            chunk_t *c,
                            vertex_t *buf,
                            const ivec3 blockPos,
                            const direction_e dir,
                            int width,
                            int height,
                            const int type) {
    ivec3 dirVec;
    memcpy(&dirVec, &directions[dir], sizeof(ivec3));
    const int texIndex = type * 4;
    memcpy(buf, blockVertices[dir], faceVerticesSize);
    switch (dir) {
        case DIR_PLUSZ:
        case DIR_MINUSZ:
            for (int i = 0; i < 6; ++i) {
                buf[i].x = (buf[i].x * (float)width + (float)blockPos[0]);
                buf[i].y = buf[i].y * (float)height + (float)blockPos[1];
                buf[i].z += (float)blockPos[2];
                buf[i].texIndex += texIndex;
                buf[i].lightValue = computeVertexLight(w, c, (int)buf[i].x, (int)buf[i].y, (int)buf[i].z, dir);
            }
            break;
        case DIR_PLUSY:
        case DIR_MINUSY:
            for (int i = 0; i < 6; ++i) {
                buf[i].x = buf[i].x * (float)width + (float)blockPos[0];
                buf[i].y += (float)blockPos[1];
                buf[i].z = buf[i].z * (float)height + (float)blockPos[2];
                buf[i].texIndex += texIndex;
                buf[i].lightValue = computeVertexLight(w, c, (int)buf[i].x, (int)buf[i].y, (int)buf[i].z, dir);
            }
            break;
        case DIR_PLUSX:
        case DIR_MINUSX:
            for (int i = 0; i < 6; ++i) {
                buf[i].x += (float)blockPos[0];
                buf[i].y = buf[i].y * (float)width + (float)blockPos[1];
                buf[i].z = buf[i].z * (float)height + (float)blockPos[2];
                buf[i].texIndex += texIndex;
                buf[i].lightValue = computeVertexLight(w, c, (int)buf[i].x, (int)buf[i].y, (int)buf[i].z, dir);
            }
            break;
    }
    return buf + 6;
}

//TODO(): Lukas, please can you write documentation for these?
/**
 * @brief Helper for greedy meshing to compute next coordinate for width and height expansion based on direction
 * @param out The output ivec3
 * @param i
 * @param j
 * @param k
 * @param dir
 * @param width
 * @param height
 */
static void getNextCoord(ivec3 out,
                        const int i,
                        const int j,
                        const int k,
                        const direction_e dir,
                        const int width,
                        const int height) {
    switch (dir) {
        case DIR_PLUSZ:
        case DIR_MINUSZ:
            out[0] = i + width;
            out[1] = j + height;
            out[2] = k;
            break;
        case DIR_PLUSY:
        case DIR_MINUSY:
            out[0] = i + width;
            out[1] = j;
            out[2] = k + height;
            break;
        case DIR_PLUSX:
        case DIR_MINUSX:
            out[0] = i;
            out[1] = j + width;
            out[2] = k + height;
            break;
        default:
            LOG_FATAL("Invalid direction enum for getHeightCoord");
    }
}

/**
 * @brief Greedy meshes in one direction, writing quads to buf
 * @param world A pointer to a world
 * @param c A pointer to a chunk
 * @param dir The direction to mesh in
 * @param buf A buffer of vertices
 * @return The updated pointer to the buffer
 */
static vertex_t *greedyMeshDirection(world_t *world, chunk_t *c, const direction_e dir, vertex_t *buf) {
    ivec3 dirVec;
    memcpy(&dirVec, &directions[dir], sizeof(ivec3));
    vertex_t *nextPtr = buf;
    bool seen[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {0};
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        for (int j = 0; j < CHUNK_SIZE; ++j) {
            for (int k = 0; k < CHUNK_SIZE; ++k) {
                ivec3 base = {i, j, k};
                const block_t type = c->blocks[i][j][k];
                if (seen[i][j][k] || type == BL_AIR || !faceIsVisible(world, c, base, dir)) {
                    continue;
                }
                seen[i][j][k] = true;
                unsigned char light = glm_imax(EXTRACT_SUN(c->lightMap[i][j][k]), EXTRACT_TORCH(c->lightMap[i][j][k]));
                int width = 1;
                int height = 1;

                // removed greedy meshing due to vertex lighting making merged faces look strange

            //     // expand width
            //     for (; width < CHUNK_SIZE; ++width) {
            //         ivec3 nextCoord;
            //         getNextCoord(nextCoord, i, j, k, dir, width, 0);
            //         ivec3 normalNextCoord;
            //         glm_ivec3_add(nextCoord, dirVec, normalNextCoord);
            //         int nx = nextCoord[0], ny = nextCoord[1], nz = nextCoord[2];
            //         if (nx < 0 || ny < 0 || nz < 0 || nx >= CHUNK_SIZE || ny >= CHUNK_SIZE || nz >= CHUNK_SIZE) {
            //             break;
            //         }
            //         if (normalNextCoord[0] >= 0 && normalNextCoord[1] >= 0 && normalNextCoord[2] >= 0 &&
            //             normalNextCoord[0] < CHUNK_SIZE && normalNextCoord[1] < CHUNK_SIZE && normalNextCoord[2] < CHUNK_SIZE) {
            //             if (light != glm_imax(EXTRACT_SUN(c->lightMap[normalNextCoord[0]][normalNextCoord[1]][normalNextCoord[2]]),
            //                 EXTRACT_TORCH(c->lightMap[normalNextCoord[0]][normalNextCoord[1]][normalNextCoord[2]]))) {
            //                 break;
            //             }
            //         }
            //         if (seen[nx][ny][nz] || c->blocks[nx][ny][nz] != type || !faceIsVisible(world, c, nextCoord, dir)) {
            //             break;
            //         }
            //         seen[nx][ny][nz] = true;
            //     }
            //     // expand height
            //     bool ok;
            //     for (; height < CHUNK_SIZE; ++height) {
            //         ok = true;
            //         for (int w = 0; w < width; ++w) {
            //             ivec3 nextCoord;
            //             getNextCoord(nextCoord, i, j, k, dir, w, height);
            //             ivec3 normalNextCoord;
            //             glm_ivec3_add(nextCoord, dirVec, normalNextCoord);
            //             int nx = nextCoord[0], ny = nextCoord[1], nz = nextCoord[2];
            //             if (nx < 0 || ny < 0 || nz < 0 || nx >= CHUNK_SIZE || ny >= CHUNK_SIZE || nz >= CHUNK_SIZE) {
            //                 ok = false;
            //                 break;
            //             }
            //             if (normalNextCoord[0] >= 0 && normalNextCoord[1] >= 0 && normalNextCoord[2] >= 0 &&
            //                 normalNextCoord[0] < CHUNK_SIZE && normalNextCoord[1] < CHUNK_SIZE && normalNextCoord[2] < CHUNK_SIZE) {
            //                 if (light != glm_imax(EXTRACT_SUN(c->lightMap[normalNextCoord[0]][normalNextCoord[1]][normalNextCoord[2]]),
            //                 EXTRACT_TORCH(c->lightMap[normalNextCoord[0]][normalNextCoord[1]][normalNextCoord[2]]))) {
            //                     ok = false;
            //                     break;
            //                 }
            //             }
            //             if (seen[nx][ny][nz] || c->blocks[nx][ny][nz] != type || !faceIsVisible(world, c, nextCoord, dir)) {
            //                 ok = false;
            //                 break;
            //             }
            //         }
            //         if (!ok) {
            //             break;
            //         }
            //         for (int w = 0; w < width; ++w) {
            //             ivec3 nextCoord;
            //             getNextCoord(nextCoord, i, j, k, dir, w, height);
            //             int nx = nextCoord[0], ny = nextCoord[1], nz = nextCoord[2];
            //             seen[nx][ny][nz] = true;
            //         }
            //     }
                nextPtr = writeFace(world, c, nextPtr, base, dir, width, height, type);
            }
        }
    }
    return nextPtr;
}

/**
 * @brief Generates a mesh for a chunk
 * @param c A pointer to a chunk
 * @param w A pointer to a world
 */
void chunk_genMesh(chunk_t *c, world_t *w) {
    const size_t bytesPerBlock = sizeof(vertex_t) * 36;
    c->vertices = malloc(CHUNK_SIZE_CUBED * bytesPerBlock);
    vertex_t *nextPtr = c->vertices;
    for (direction_e dir = 0; dir < 6; ++dir) {
        nextPtr = greedyMeshDirection(w, c, dir, nextPtr);
    }
    const GLsizeiptr sizeToWrite = (GLsizeiptr)sizeof(vertex_t) * (nextPtr - c->vertices);
    c->meshVertices = (int)sizeToWrite / (int)sizeof(vertex_t);
}

/**
 * @brief Writes the mesh generated by chunk_genMesh to opengl
 * @param c A pointer to a chunk
 * @param w A pointer to a world
 */
bool chunk_createMesh(chunk_t *c, world_t *w) {
    if (!c->verticesValid) return false;

    if (c->vbo == -1) {
        glGenBuffers(1, &c->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
        glGenVertexArrays(1, &c->vao);
    }

    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(c->meshVertices * sizeof(vertex_t)), c->vertices, GL_STATIC_DRAW);

    glBindVertexArray(c->vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_INT, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    c->verticesValid = false;
    free(c->vertices);
    return true;
}