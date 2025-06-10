#include "vertices.h"

#define ATLAS_LENGTH 96
#define ATLAS_HEIGHT 64
#define TEXTURE_LENGTH 16
#define TEXTURE_HEIGHT 16

ivec3 directions[6] = {
    (ivec3){ 1, 0, 0 },
    (ivec3){ -1, 0, 0 },
    (ivec3){ 0, 1, 0 },
    (ivec3){ 0, -1, 0 },
    (ivec3){ 0, 0, 1 },
    (ivec3){ 0, 0, -1 },
};

const vertex_t backFaceVertices[] = {
    {0.0f, 0.0f, 0.0f, 2},
    {1.0f, 0.0f, 0.0f, 2},
    {1.0f,  1.0f, 0.0f, 2},
    {1.0f,  1.0f, 0.0f, 2},
    {0.0f,  1.0f, 0.0f, 2},
    {0.0f, 0.0f, 0.0f, 2},
};

const vertex_t frontFaceVertices[] = {
    {0.0f, 0.0f,  1.0f, 2},
    {1.0f, 0.0f,  1.0f, 2},
    {1.0f,  1.0f,  1.0f, 2},
    {1.0f,  1.0f,  1.0f,  2},
    {0.0f,  1.0f,  1.0f, 2},
    {0.0f, 0.0f,  1.0f, 2},
};

const vertex_t leftFaceVertices[] = {
    {0.0f,  1.0f,  1.0f, 2},
    {0.0f,  1.0f, 0.0f, 2},
    {0.0f, 0.0f, 0.0f, 2},
    {0.0f, 0.0f, 0.0f, 2},
    {0.0f, 0.0f,  1.0f, 2},
    {0.0f,  1.0f,  1.0f, 2},
};

const vertex_t rightFaceVertices[] = {
    {1.0f,  1.0f,  1.0f, 2},
    {1.0f,  1.0f, 0.0f, 2},
    {1.0f, 0.0f, 0.0f, 2},
    {1.0f, 0.0f, 0.0f, 2},
    {1.0f, 0.0f,  1.0f, 2},
    {1.0f,  1.0f,  1.0f, 2},
};

const vertex_t bottomFaceVertices[] = {
    {0.0f, 0.0f, 0.0f, 1},
    {1.0f, 0.0f, 0.0f,  1},
    {1.0f, 0.0f,  1.0f, 1},
    {1.0f, 0.0f,  1.0f, 1},
    {0.0f, 0.0f,  1.0f, 1},
    {0.0f, 0.0f, 0.0f,  1},
};

const vertex_t topFaceVertices[] = {
    {0.0f,  1.0f, 0.0f, 3},
    {1.0f,  1.0f, 0.0f, 3},
    {1.0f,  1.0f,  1.0f, 3},
    {1.0f,  1.0f,  1.0f, 3},
    {0.0f,  1.0f,  1.0f, 3},
    {0.0f,  1.0f, 0.0f, 3},
};

const unsigned int faceVerticesSize = sizeof(leftFaceVertices);

const vertex_t blockVertices[6][6] = {
    {    {1.0f,  1.0f,  1.0f, 2},
{1.0f,  1.0f, 0.0f, 2},
{1.0f, 0.0f, 0.0f, 2},
{1.0f, 0.0f, 0.0f, 2},
{1.0f, 0.0f,  1.0f, 2},
{1.0f,  1.0f,  1.0f, 2},},
{    {0.0f,  1.0f,  1.0f, 2},
{0.0f,  1.0f, 0.0f, 2},
{0.0f, 0.0f, 0.0f, 2},
{0.0f, 0.0f, 0.0f, 2},
{0.0f, 0.0f,  1.0f, 2},
{0.0f,  1.0f,  1.0f, 2}},
{    {0.0f,  1.0f, 0.0f, 3},
{1.0f,  1.0f, 0.0f, 3},
{1.0f,  1.0f,  1.0f, 3},
{1.0f,  1.0f,  1.0f, 3},
{0.0f,  1.0f,  1.0f, 3},
{0.0f,  1.0f, 0.0f, 3}},
{    {0.0f, 0.0f, 0.0f, 1},
{1.0f, 0.0f, 0.0f,  1},
{1.0f, 0.0f,  1.0f, 1},
{1.0f, 0.0f,  1.0f, 1},
{0.0f, 0.0f,  1.0f, 1},
{0.0f, 0.0f, 0.0f,  1},},
{    {0.0f, 0.0f,  1.0f, 2},
{1.0f, 0.0f,  1.0f, 2},
{1.0f,  1.0f,  1.0f, 2},
{1.0f,  1.0f,  1.0f,  2},
{0.0f,  1.0f,  1.0f, 2},
{0.0f, 0.0f,  1.0f, 2}},
    {    {0.0f, 0.0f, 0.0f, 2},
    {1.0f, 0.0f, 0.0f, 2},
    {1.0f,  1.0f, 0.0f, 2},
    {1.0f,  1.0f, 0.0f, 2},
    {0.0f,  1.0f, 0.0f, 2},
    {0.0f, 0.0f, 0.0f, 2}},
};

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