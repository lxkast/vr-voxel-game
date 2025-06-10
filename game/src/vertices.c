#include "vertices.h"

const float backFaceVertices[] = {
    0.0f, 0.0f, 0.0f,  0.0f, 0.5f,  // U: 16/96, V: 0/16
    1.0f, 0.0f, 0.0f,  1.0f, 0.5f,  // U: 32/96, V: 0/16
    1.0f,  1.0f, 0.0f,  1.0f, 0.75f,  // U: 32/96, V: 16/16
    1.0f,  1.0f, 0.0f,  1.0f, 0.75f,  // U: 32/96, V: 16/16
    0.0f,  1.0f, 0.0f,  0.0f, 0.75f,  // U: 16/96, V: 16/16
    0.0f, 0.0f, 0.0f,  0.0f, 0.5f,  // U: 16/96, V: 0/16
};

const float frontFaceVertices[] = {
    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,  // U: 16/96, V: 0/16
    1.0f, 0.0f,  1.0f,  1.0f, 0.5f,  // U: 32/96, V: 0/16
    1.0f,  1.0f,  1.0f,  1.0f, 0.75f,  // U: 32/96, V: 16/16
    1.0f,  1.0f,  1.0f,  1.0f, 0.75f,  // U: 32/96, V: 16/16
    0.0f,  1.0f,  1.0f,  0.0f, 0.75f,  // U: 16/96, V: 16/16
    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,  // U: 16/96, V: 0/16
};

const float leftFaceVertices[] = {
    0.0f,  1.0f,  1.0f,  0.0f, 0.75f,  // U: 16/96, V: 16/16
    0.0f,  1.0f, 0.0f,  1.0f, 0.75f,  // U: 32/96, V: 16/16
    0.0f, 0.0f, 0.0f,  1.0f, 0.5f,  // U: 32/96, V: 0/16
    0.0f, 0.0f, 0.0f,  1.0f, 0.5f,  // U: 32/96, V: 0/16
    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,  // U: 16/96, V: 0/16
    0.0f,  1.0f,  1.0f,  0.0f, 0.75f,  // U: 16/96, V: 16/16
};

const float rightFaceVertices[] = {
    1.0f,  1.0f,  1.0f,  0.0f, 0.75f,  // U: 16/96, V: 16/16
    1.0f,  1.0f, 0.0f,  1.0f, 0.75f,  // U: 32/96, V: 16/16
    1.0f, 0.0f, 0.0f,  1.0f, 0.5f,  // U: 32/96, V: 0/16
    1.0f, 0.0f, 0.0f,  1.0f, 0.5f,  // U: 32/96, V: 0/16
    1.0f, 0.0f,  1.0f,  0.0f, 0.5f,  // U: 16/96, V: 0/16
    1.0f,  1.0f,  1.0f,  0.0f, 0.75f,  // U: 16/96, V: 16/16
};

const float bottomFaceVertices[] = {
    0.0f, 0.0f, 0.0f,  0.0f, 0.25f,  // U: 16/96, V: 0/16
    1.0f, 0.0f, 0.0f,  1.0f, 0.25f,  // U: 32/96, V: 0/16
    1.0f, 0.0f,  1.0f,  1.0f, 0.5f,  // U: 32/96, V: 16/16
    1.0f, 0.0f,  1.0f,  1.0f, 0.5f,  // U: 32/96, V: 16/16
    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,  // U: 16/96, V: 16/16
    0.0f, 0.0f, 0.0f,  0.0f, 0.25f,  // U: 16/96, V: 0/16
};

const float topFaceVertices[] = {
    0.0f,  1.0f, 0.0f,  0.0f, 1.0f,  // U: 0/96, V: 16/16
    1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // U: 16/96, V: 16/16
    1.0f,  1.0f,  1.0f,  1.0f, 0.75f,  // U: 16/96, V: 0/16
    1.0f,  1.0f,  1.0f,  1.0f, 0.75f,  // U: 16/96, V: 0/16
    0.0f,  1.0f,  1.0f,  0.0f, 0.75f,  // U: 0/96, V: 0/16
    0.0f,  1.0f, 0.0f,  0.0f, 1.0f   // U: 0/96, V: 16/16
};

const unsigned int faceVerticesSize = sizeof(leftFaceVertices);

const float cubeVertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,  // 0
     0.5f, -0.5f,  0.5f,  // 1
     0.5f,  0.5f,  0.5f,  // 2
    -0.5f,  0.5f,  0.5f,  // 3
    // Back face
    -0.5f, -0.5f, -0.5f,  // 4
     0.5f, -0.5f, -0.5f,  // 5
     0.5f,  0.5f, -0.5f,  // 6
    -0.5f,  0.5f, -0.5f   // 7
};
const unsigned int cubeVerticesSize = sizeof(cubeVertices);

const unsigned int cubeIndices[] = {
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
const unsigned int cubeIndicesSize = sizeof(cubeIndices);

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