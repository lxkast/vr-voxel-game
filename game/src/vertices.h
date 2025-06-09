#pragma once
#include <cglm/cglm.h>

typedef struct {
    float x,y,z;
    int texIndex;
} vertex_t;

extern ivec3 directions[6];

typedef enum {
    DIR_PLUSZ,
    DIR_MINUSZ,
    DIR_PLUSY,
    DIR_MINUSY,
    DIR_PLUSX,
    DIR_MINUSX
} direction_e;

extern const float cubeVertices[];
extern const unsigned int cubeVerticesSize;

extern const unsigned int cubeIndices[];
extern const unsigned int cubeIndicesSize;

extern const vertex_t blockVertices[6][6];

extern const unsigned int faceVerticesSize;

extern const float quadVertices[];

extern const unsigned int quadVerticesSize;

extern const unsigned int quadIndices[];

extern const unsigned int quadIndicesSize;