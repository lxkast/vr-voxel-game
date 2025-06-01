#include "vertices.h"

#define ATLAS_LENGTH 96
#define ATLAS_HEIGHT 64
#define TEXTURE_LENGTH 16
#define TEXTURE_HEIGHT 16

const float grassVertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.166667f, 0.0f,  // U: 16/96, V: 0/16
     0.5f, -0.5f, -0.5f,  0.333333f, 0.0f,  // U: 32/96, V: 0/16
     0.5f,  0.5f, -0.5f,  0.333333f, 1.0f,  // U: 32/96, V: 16/16
     0.5f,  0.5f, -0.5f,  0.333333f, 1.0f,  // U: 32/96, V: 16/16
    -0.5f,  0.5f, -0.5f,  0.166667f, 1.0f,  // U: 16/96, V: 16/16
    -0.5f, -0.5f, -0.5f,  0.166667f, 0.0f,  // U: 16/96, V: 0/16

    // Front face
    -0.5f, -0.5f,  0.5f,  0.166667f, 0.0f,  // U: 16/96, V: 0/16
     0.5f, -0.5f,  0.5f,  0.333333f, 0.0f,  // U: 32/96, V: 0/16
     0.5f,  0.5f,  0.5f,  0.333333f, 1.0f,  // U: 32/96, V: 16/16
     0.5f,  0.5f,  0.5f,  0.333333f, 1.0f,  // U: 32/96, V: 16/16
    -0.5f,  0.5f,  0.5f,  0.166667f, 1.0f,  // U: 16/96, V: 16/16
    -0.5f, -0.5f,  0.5f,  0.166667f, 0.0f,  // U: 16/96, V: 0/16

    // Left face
    -0.5f,  0.5f,  0.5f,  0.166667f, 1.0f,  // U: 16/96, V: 16/16
    -0.5f,  0.5f, -0.5f,  0.333333f, 1.0f,  // U: 32/96, V: 16/16
    -0.5f, -0.5f, -0.5f,  0.333333f, 0.0f,  // U: 32/96, V: 0/16
    -0.5f, -0.5f, -0.5f,  0.333333f, 0.0f,  // U: 32/96, V: 0/16
    -0.5f, -0.5f,  0.5f,  0.166667f, 0.0f,  // U: 16/96, V: 0/16
    -0.5f,  0.5f,  0.5f,  0.166667f, 1.0f,  // U: 16/96, V: 16/16

    // Right face
     0.5f,  0.5f,  0.5f,  0.166667f, 1.0f,  // U: 16/96, V: 16/16
     0.5f,  0.5f, -0.5f,  0.333333f, 1.0f,  // U: 32/96, V: 16/16
     0.5f, -0.5f, -0.5f,  0.333333f, 0.0f,  // U: 32/96, V: 0/16
     0.5f, -0.5f, -0.5f,  0.333333f, 0.0f,  // U: 32/96, V: 0/16
     0.5f, -0.5f,  0.5f,  0.166667f, 0.0f,  // U: 16/96, V: 0/16
     0.5f,  0.5f,  0.5f,  0.166667f, 1.0f,  // U: 16/96, V: 16/16

    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.166667f, 0.0f,  // U: 16/96, V: 0/16
     0.5f, -0.5f, -0.5f,  0.333333f, 0.0f,  // U: 32/96, V: 0/16
     0.5f, -0.5f,  0.5f,  0.333333f, 1.0f,  // U: 32/96, V: 16/16
     0.5f, -0.5f,  0.5f,  0.333333f, 1.0f,  // U: 32/96, V: 16/16
    -0.5f, -0.5f,  0.5f,  0.166667f, 1.0f,  // U: 16/96, V: 16/16
    -0.5f, -0.5f, -0.5f,  0.166667f, 0.0f,  // U: 16/96, V: 0/16

    // Top face (Grass texture)
    -0.5f,  0.5f, -0.5f,  0.0f,        1.0f,  // U: 0/96, V: 16/16
     0.5f,  0.5f, -0.5f,  0.166667f, 1.0f,  // U: 16/96, V: 16/16
     0.5f,  0.5f,  0.5f,  0.166667f, 0.0f,  // U: 16/96, V: 0/16
     0.5f,  0.5f,  0.5f,  0.166667f, 0.0f,  // U: 16/96, V: 0/16
    -0.5f,  0.5f,  0.5f,  0.0f,        0.0f,  // U: 0/96, V: 0/16
    -0.5f,  0.5f, -0.5f,  0.0f,        1.0f   // U: 0/96, V: 16/16
};

const unsigned int grassVerticesSize = sizeof(grassVertices);

const float chunkVertices[] = {
 // Front face
 0.0f, 0.0f,  16.0f,  // 0
  16.0f, 0.0f,  16.0f,  // 1
  16.0f,  16.0f,  16.0f,  // 2
 0.0f,  16.0f,  16.0f,  // 3
 // Back face
 0.0f, 0.0f, 0.0f,  // 4
  16.0f, 0.0f, 0.0f,  // 5
  16.0f,  16.0f, 0.0f,  // 6
 0.0f,  16.0f, 0.0f   // 7
};

const unsigned int chunkVerticesSize = sizeof(chunkVertices);

const unsigned int chunkIndices[] = {
    // Front face
    0, 1, 2,
    2, 3, 0,
    // Right face
    1, 5, 6,
    6, 2, 1,
    // Back face
    5, 4, 7,
    7, 6, 5,
    // Left face
    4, 0, 3,
    3, 7, 4,
    // Top face
    3, 2, 6,
    6, 7, 3,
    // Bottom face
    4, 5, 1,
    1, 0, 4
};
const unsigned int chunkIndicesSize = sizeof(chunkIndices);

const float quadVertices[] = {
    // positions   // texCoords

    // left side
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    0.0f, -1.0f,  0.5f, 0.0f,

    -1.0f, 1.0f,   0.0f, 1.0f,
    0.0f, -1.0f,   0.5f, 0.0f,
    0.0f, 1.0f,    0.5f, 1.0f,

    // right side

    0.0f,  1.0f,  0.5f, 1.0f,
    0.0f, -1.0f,  0.5f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    0.0f, 1.0f,   0.5f, 1.0f,
    1.0f, -1.0f,   1.0f, 0.0f,
    1.0f, 1.0f,    1.0f, 1.0f,
};

const unsigned int quadVerticesSize = sizeof(quadVertices);