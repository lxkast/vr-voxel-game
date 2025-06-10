#include <cglm/cglm.h>
#include <stdlib.h>
#include <string.h>
#include "chunk.h"
#include "vertices.h"

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
    return c->blocks[neighbourPos[0]][neighbourPos[1]][neighbourPos[2]] == BL_AIR;
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

/**
 * @brief Creates the mesh from a chunk
 * @param c A pointer to a chunk
 */
void chunk_createMesh(chunk_t *c) {
    const size_t bytesPerBlock = sizeof(vertex_t) * 36;
    bool seen[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {0};

    vertex_t *buf = malloc(CHUNK_SIZE_CUBED * bytesPerBlock);
    vertex_t *nextPtr = buf;
    direction_e dir;
    // positive Z
    dir = DIR_PLUSZ;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                block_t type = c->blocks[i][j][k];
                if (seen[i][j][k]) {
                    continue;
                }
                if (type == BL_AIR) {
                    continue;
                }

                if (!faceIsVisible(c, (ivec3){i, j, k}, dir)) {
                    continue;
                }
                seen[i][j][k] = true;
                int width = 1;
                int height = 1;
                while ( i + width < CHUNK_SIZE
                        && faceIsVisible(c, (ivec3){i + width, j, k}, dir)
                        && !seen[i + width][j][k]
                        && c->blocks[i + width][j][k] == type) {
                    seen[i + width][j][k] = true;
                    width++;
                }
                for (int areaHeight = j + 1; areaHeight < CHUNK_SIZE; areaHeight++) {
                    bool validSpace = true;
                    for (int x = i; x < i + width; ++x) {
                        if (seen[x][areaHeight][k]
                            || !faceIsVisible(c, (ivec3){x, areaHeight, k}, dir)
                            || c->blocks[x][areaHeight][k] != type) {
                            validSpace = false;
                            break;
                        }
                    }
                    if (validSpace) {
                        height++;
                        for (int x = i; x < i + width; ++x) {
                            seen[x][areaHeight][k] = true;
                        }
                    } else {
                        break;
                    }
                }
                nextPtr = writeVertex(nextPtr, (ivec3){i, j, k}, dir, width, height, type);
            }
        }
    }
    memset(seen, false, sizeof(seen));
    // negative Z
    dir = DIR_MINUSZ;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                block_t type = c->blocks[i][j][k];
                if (seen[i][j][k]) {
                    continue;
                }
                if (type == BL_AIR) {
                    continue;
                }

                if (!faceIsVisible(c, (ivec3){i, j, k}, dir)) {
                    continue;
                }
                seen[i][j][k] = true;
                int width = 1;
                int height = 1;
                while ( i + width < CHUNK_SIZE
                        && faceIsVisible(c, (ivec3){i + width, j, k}, dir)
                        && !seen[i + width][j][k]
                        && c->blocks[i + width][j][k] == type) {
                    seen[i + width][j][k] = true;
                    width++;
                        }
                for (int areaHeight = j + 1; areaHeight < CHUNK_SIZE; areaHeight++) {
                    bool validSpace = true;
                    for (int x = i; x < i + width; ++x) {
                        if (seen[x][areaHeight][k]
                            || !faceIsVisible(c, (ivec3){x, areaHeight, k}, dir)
                            || c->blocks[x][areaHeight][k] != type) {
                            validSpace = false;
                            break;
                            }
                    }
                    if (validSpace) {
                        height++;
                        for (int x = i; x < i + width; ++x) {
                            seen[x][areaHeight][k] = true;
                        }
                    } else {
                        break;
                    }
                }
                nextPtr = writeVertex(nextPtr, (ivec3){i, j, k}, dir, width, height, type);
            }
        }
    }
    memset(seen, false, sizeof(seen));
    // positive Y
    dir = DIR_PLUSY;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                block_t type = c->blocks[i][j][k];
                if (seen[i][j][k]) {
                    continue;
                }
                if (type == BL_AIR) {
                    continue;
                }

                if (!faceIsVisible(c, (ivec3){i, j, k}, dir)) {
                    continue;
                }
                seen[i][j][k] = true;
                int width = 1;
                int height = 1;
                while ( i + width < CHUNK_SIZE
                        && faceIsVisible(c, (ivec3){i + width, j, k}, dir)
                        && !seen[i + width][j][k]
                        && c->blocks[i + width][j][k] == type) {
                    seen[i + width][j][k] = true;
                    width++;
                        }
                for (int areaHeight = k + 1; areaHeight < CHUNK_SIZE; areaHeight++) {
                    bool validSpace = true;
                    for (int x = i; x < i + width; ++x) {
                        if (seen[x][j][areaHeight]
                            || !faceIsVisible(c, (ivec3){x, j, areaHeight}, dir)
                            || c->blocks[x][j][areaHeight] != type) {
                            validSpace = false;
                            break;
                            }
                    }
                    if (validSpace) {
                        height++;
                        for (int x = i; x < i + width; ++x) {
                            seen[x][j][areaHeight] = true;
                        }
                    } else {
                        break;
                    }
                }
                nextPtr = writeVertex(nextPtr, (ivec3){i, j, k}, dir, width, height, type);
            }
        }
    }
    memset(seen, false, sizeof(seen));
    dir = DIR_MINUSY;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                block_t type = c->blocks[i][j][k];
                if (seen[i][j][k]) {
                    continue;
                }
                if (type == BL_AIR) {
                    continue;
                }

                if (!faceIsVisible(c, (ivec3){i, j, k}, dir)) {
                    continue;
                }
                seen[i][j][k] = true;
                int width = 1;
                int height = 1;
                while ( i + width < CHUNK_SIZE
                        && faceIsVisible(c, (ivec3){i + width, j, k}, dir)
                        && !seen[i + width][j][k]
                        && c->blocks[i + width][j][k] == type) {
                    seen[i + width][j][k] = true;
                    width++;
                        }
                for (int areaHeight = k + 1; areaHeight < CHUNK_SIZE; areaHeight++) {
                    bool validSpace = true;
                    for (int x = i; x < i + width; ++x) {
                        if (seen[x][j][areaHeight]
                            || !faceIsVisible(c, (ivec3){x, j, areaHeight}, dir)
                            || c->blocks[x][j][areaHeight] != type) {
                            validSpace = false;
                            break;
                            }
                    }
                    if (validSpace) {
                        height++;
                        for (int x = i; x < i + width; ++x) {
                            seen[x][j][areaHeight] = true;
                        }
                    } else {
                        break;
                    }
                }
                nextPtr = writeVertex(nextPtr, (ivec3){i, j, k}, dir, width, height, type);
            }
        }
    }
    memset(seen, false, sizeof(seen));
    dir = DIR_PLUSX;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                block_t type = c->blocks[i][j][k];
                if (seen[i][j][k]) {
                    continue;
                }
                if (type == BL_AIR) {
                    continue;
                }

                if (!faceIsVisible(c, (ivec3){i, j, k}, dir)) {
                    continue;
                }
                seen[i][j][k] = true;
                int width = 1;
                int height = 1;
                while ( j + width < CHUNK_SIZE
                        && faceIsVisible(c, (ivec3){i, j + width, k}, dir)
                        && !seen[i][j + width][k]
                        && c->blocks[i][j + width][k] == type) {
                    seen[i][j + width][k] = true;
                    width++;
                        }
                for (int areaHeight = k + 1; areaHeight < CHUNK_SIZE; areaHeight++) {
                    bool validSpace = true;
                    for (int y = j; y < j + width; ++y) {
                        if (seen[i][y][areaHeight]
                            || !faceIsVisible(c, (ivec3){i, y, areaHeight}, dir)
                            || c->blocks[i][y][areaHeight] != type) {
                            validSpace = false;
                            break;
                            }
                    }
                    if (validSpace) {
                        height++;
                        for (int y = j; y < j + width; ++y) {
                            seen[i][y][areaHeight] = true;
                        }
                    } else {
                        break;
                    }
                }
                nextPtr = writeVertex(nextPtr, (ivec3){i, j, k}, dir, width, height, type);
            }
        }
    }
    memset(seen, false, sizeof(seen));
    dir = DIR_MINUSX;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                block_t type = c->blocks[i][j][k];
                if (seen[i][j][k]) {
                    continue;
                }
                if (type == BL_AIR) {
                    continue;
                }

                if (!faceIsVisible(c, (ivec3){i, j, k}, dir)) {
                    continue;
                }
                seen[i][j][k] = true;
                int width = 1;
                int height = 1;
                while ( j + width < CHUNK_SIZE
                        && faceIsVisible(c, (ivec3){i, j + width, k}, dir)
                        && !seen[i][j + width][k]
                        && c->blocks[i][j + width][k] == type) {
                    seen[i][j + width][k] = true;
                    width++;
                        }
                for (int areaHeight = k + 1; areaHeight < CHUNK_SIZE; areaHeight++) {
                    bool validSpace = true;
                    for (int y = j; y < j + width; ++y) {
                        if (seen[i][y][areaHeight]
                            || !faceIsVisible(c, (ivec3){i, y, areaHeight}, dir)
                            || c->blocks[i][y][areaHeight] != type) {
                            validSpace = false;
                            break;
                            }
                    }
                    if (validSpace) {
                        height++;
                        for (int y = j; y < j + width; ++y) {
                            seen[i][y][areaHeight] = true;
                        }
                    } else {
                        break;
                    }
                }
                nextPtr = writeVertex(nextPtr, (ivec3){i, j, k}, dir, width, height, type);
            }
        }
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