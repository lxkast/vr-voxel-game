#include <logging.h>
#include <cglm/cglm.h>
#include <stdlib.h>
#include <string.h>
#include "chunk.h"
#include "lighting.h"
#include "vertices.h"

// helper to check if a block's face neighbours air or the chunk edge
static bool faceIsVisible(chunk_t *c, ivec3 blockPos, direction_e dir) {
    ivec3 dirVec;
    memcpy(&dirVec, &directions[dir], sizeof(ivec3));
    ivec3 neighbourPos;
    glm_ivec3_add(blockPos, dirVec, neighbourPos);
    for (int i = 0; i < 3; i++) {
        if (neighbourPos[i] < 0 || neighbourPos[i] >= CHUNK_SIZE) {
            return true;
        }
    }
    block_t type = c->blocks[neighbourPos[0]][neighbourPos[1]][neighbourPos[2]];
    return type == BL_AIR || type == BL_LEAF;
}

// writes vertices of a face specified by buf
// returns pointer to next free position in the buffer
static vertex_t *writeVertex(vertex_t *buf, ivec3 blockPos, direction_e dir, int width, int height, int type) {
    ivec3 dirVec;
    memcpy(&dirVec, &directions[dir], sizeof(ivec3));
    int texIndex = type * 4;
    memcpy(buf, blockVertices[dir], faceVerticesSize);
    switch (dir) {
        case DIR_PLUSZ:
        case DIR_MINUSZ:
            for (int i = 0; i < 6; ++i) {
                buf[i].x = buf[i].x * width + blockPos[0];
                buf[i].y = buf[i].y * height + blockPos[1];
                buf[i].z += blockPos[2];
                buf[i].texIndex += texIndex;
            }
            break;
        case DIR_PLUSY:
        case DIR_MINUSY:
            for (int i = 0; i < 6; ++i) {
                buf[i].x = buf[i].x * width + blockPos[0];
                buf[i].y += blockPos[1];
                buf[i].z = buf[i].z * height + blockPos[2];
                buf[i].texIndex += texIndex;
            }
            break;
        case DIR_PLUSX:
        case DIR_MINUSX:
            for (int i = 0; i < 6; ++i) {
                buf[i].x += blockPos[0];
                buf[i].y = buf[i].y * width + blockPos[1];
                buf[i].z = buf[i].z * height + blockPos[2];
                buf[i].texIndex += texIndex;
            }
            break;
    }
    return buf + 6;
}

// helper to compute next coordinate for width and height expansion based on direction
static void getNextCoord(ivec3 out, int i, int j, int k, direction_e dir, int width, int height) {
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

// greedy meshing in one direction, writes quads to buf, returns updated pointer
static vertex_t *greedyMeshDirection(chunk_t *c, direction_e dir, vertex_t *buf) {
    vertex_t *nextPtr = buf;
    bool seen[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {0};
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        for (int j = 0; j < CHUNK_SIZE; ++j) {
            for (int k = 0; k < CHUNK_SIZE; ++k) {
                ivec3 base = {i, j, k};
                block_t type = c->blocks[i][j][k];
                if (seen[i][j][k] || type == BL_AIR || !faceIsVisible(c, base, dir)) {
                    continue;
                }
                seen[i][j][k] = true;
                int width = 1;
                int height = 1;
                // expand width
                for (; width < CHUNK_SIZE; ++width) {
                    ivec3 nextCoord;
                    getNextCoord(nextCoord, i, j, k, dir, width, 0);
                    int nx = nextCoord[0], ny = nextCoord[1], nz = nextCoord[2];
                    if (nx < 0 || ny < 0 || nz < 0 || nx >= CHUNK_SIZE || ny >= CHUNK_SIZE || nz >= CHUNK_SIZE) {
                        break;
                    }
                    if (seen[nx][ny][nz] || c->blocks[nx][ny][nz] != type || !faceIsVisible(c, nextCoord, dir)) {
                        break;
                    }
                    seen[nx][ny][nz] = true;
                }
                // expand height
                bool ok;
                for (; height < CHUNK_SIZE; ++height) {
                    ok = true;
                    for (int w = 0; w < width; ++w) {
                        ivec3 nextCoord;
                        getNextCoord(nextCoord, i, j, k, dir, w, height);
                        int nx = nextCoord[0], ny = nextCoord[1], nz = nextCoord[2];
                        if (nx < 0 || ny < 0 || nz < 0 || nx >= CHUNK_SIZE || ny >= CHUNK_SIZE || nz >= CHUNK_SIZE) {
                            ok = false;
                            break;
                        }
                        if (seen[nx][ny][nz] || c->blocks[nx][ny][nz] != type || !faceIsVisible(c, nextCoord, dir)) {
                            ok = false;
                            break;
                        }
                    }
                    if (!ok) {
                        break;
                    }
                    for (int w = 0; w < width; ++w) {
                        ivec3 nextCoord;
                        getNextCoord(nextCoord, i, j, k, dir, w, height);
                        int nx = nextCoord[0], ny = nextCoord[1], nz = nextCoord[2];
                        seen[nx][ny][nz] = true;
                    }
                }
                nextPtr = writeVertex(nextPtr, base, dir, width, height, type);
            }
        }
    }
    return nextPtr;
}

/**
 * @brief Creates the mesh from a chunk
 * @param c A pointer to a chunk
 */
void chunk_createMesh(chunk_t *c) {
    chunk_processLighting(c);
    const size_t bytesPerBlock = sizeof(vertex_t) * 36;

    vertex_t *buf = malloc(CHUNK_SIZE_CUBED * bytesPerBlock);
    vertex_t *nextPtr = buf;
    for (direction_e dir = 0; dir < 6; ++dir) {
        nextPtr = greedyMeshDirection(c, dir, nextPtr);
    }
    const GLsizeiptr sizeToWrite = sizeof(vertex_t) * (nextPtr - buf);
    c->meshVertices = sizeToWrite / sizeof(vertex_t);

    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeToWrite, buf, GL_STATIC_DRAW);

    glBindVertexArray(c->vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribIPointer(1, 1, GL_INT, 4 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    free(buf);
}