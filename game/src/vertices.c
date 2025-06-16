#include "vertices.h"

ivec3 directions[6] = {
    { 1, 0, 0 },
    { -1, 0, 0 },
    { 0, 1, 0 },
    { 0, -1, 0 },
    { 0, 0, 1 },
    { 0, 0, -1 },
};

const unsigned int faceVerticesSize = 6 * sizeof(vertex_t);

// vertices used for rendering blocks in chunks
const vertex_t blockVertices[6][6] = {
    {
        {1.0f, 0.0f, 0.0f, 2},
        {1.0f,  1.0f, 0.0f, 2},
        {1.0f,  1.0f,  1.0f, 2},
        {1.0f,  1.0f,  1.0f, 2},
        {1.0f, 0.0f,  1.0f, 2},
        {1.0f, 0.0f, 0.0f, 2},
    },
    {
        {0.0f,  1.0f,  1.0f, 2},
        {0.0f,  1.0f, 0.0f, 2},
        {0.0f, 0.0f, 0.0f, 2},
        {0.0f, 0.0f, 0.0f, 2},
        {0.0f, 0.0f,  1.0f, 2},
        {0.0f,  1.0f,  1.0f, 2}
    },
    {
        {1.0f,  1.0f,  1.0f, 3},
        {1.0f,  1.0f, 0.0f, 3},
        {0.0f,  1.0f, 0.0f, 3},
        {0.0f,  1.0f,  1.0f, 3},
        {1.0f,  1.0f,  1.0f, 3},
        {0.0f,  1.0f, 0.0f, 3}
    },
    {
        {0.0f, 0.0f, 0.0f, 1},
        {1.0f, 0.0f, 0.0f,  1},
        {1.0f, 0.0f,  1.0f, 1},
        {1.0f, 0.0f,  1.0f, 1},
        {0.0f, 0.0f,  1.0f, 1},
        {0.0f, 0.0f, 0.0f,  1},
    },
    {
        {0.0f, 0.0f,  1.0f, 2},
        {1.0f, 0.0f,  1.0f, 2},
        {1.0f,  1.0f,  1.0f, 2},
        {1.0f,  1.0f,  1.0f,  2},
        {0.0f,  1.0f,  1.0f, 2},
        {0.0f, 0.0f,  1.0f, 2}
    },
    {
        {1.0f, 0.0f, 0.0f, 2},
        {0.0f, 0.0f, 0.0f, 2},
        {1.0f,  1.0f, 0.0f, 2},
        {0.0f,  1.0f, 0.0f, 2},
        {1.0f,  1.0f, 0.0f, 2},
        {0.0f, 0.0f, 0.0f, 2}
    },
};

// vertices used for rendering item entities (dropped blocks)
const float itemBlockVertices[] = {
    // right
    1.0f, 0.0f, 0.0f,  1.0f, 0.5f,
    1.0f,  1.0f, 0.0f,  1.0f, 0.75f,
    1.0f,  1.0f,  1.0f,  0.0f, 0.75f,
    1.0f,  1.0f,  1.0f,  0.0f, 0.75f,
    1.0f, 0.0f,  1.0f,  0.0f, 0.5f,
    1.0f, 0.0f, 0.0f,  1.0f, 0.5f,
    // left
    0.0f,  1.0f,  1.0f,  0.0f, 0.75f,
    0.0f,  1.0f, 0.0f,  1.0f, 0.75f,
    0.0f, 0.0f, 0.0f,  1.0f, 0.5f,
    0.0f, 0.0f, 0.0f,  1.0f, 0.5f,
    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,
    0.0f,  1.0f,  1.0f,  0.0f, 0.75f,
    // top
    1.0f,  1.0f,  1.0f,  1.0f, 0.75f,
    1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
    0.0f,  1.0f, 0.0f,  0.0f, 1.0f,
    0.0f,  1.0f, 0.0f,  0.0f, 1.0f,
    0.0f,  1.0f,  1.0f,  0.0f, 0.75f,
    1.0f,  1.0f,  1.0f,  1.0f, 0.75f,
    // bottom
    1.0f, 0.0f, 0.0f,  1.0f, 0.25f,
    1.0f, 0.0f,  1.0f,  1.0f, 0.5f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.25f,
    1.0f, 0.0f,  1.0f,  1.0f, 0.5f,
    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.25f,
    // front
    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,
    1.0f, 0.0f,  1.0f,  1.0f, 0.5f,
    1.0f,  1.0f,  1.0f,  1.0f, 0.75f,
    1.0f,  1.0f,  1.0f,  1.0f, 0.75f,
    0.0f,  1.0f,  1.0f,  0.0f, 0.75f,
    0.0f, 0.0f,  1.0f,  0.0f, 0.5f,
    // back
    1.0f, 0.0f, 0.0f,  1.0f, 0.5f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.5f,
    1.0f,  1.0f, 0.0f,  1.0f, 0.75f,
    0.0f,  1.0f, 0.0f,  0.0f, 0.75f,
    1.0f,  1.0f, 0.0f,  1.0f, 0.75f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.5f,
};

const unsigned int itemBlockVerticesSize = sizeof(itemBlockVertices);

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

const float squareVertices[] = {
    -1.f, 1.f, 0.f,    0.f, 1.f,
    -1.f, -1.f, 0.f,    0.f, 0.f,
    1.f, -1.f, 0.f,    1.f, 0.f,

    -1.f, 1.f, 0.f,    0.f, 1.f,
    1.f, -1.f, 0.f,    1.f, 0.f,
    1.f, 1.f, 0.f,    1.f, 1.f
};
const unsigned int squareVerticesSize = sizeof(squareVertices);

