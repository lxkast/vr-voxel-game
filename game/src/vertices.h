#pragma once
#include <cglm/cglm.h>

typedef struct {
    float x,y,z;
    int texIndex;
} vertex_t;

extern ivec3 directions[6];

typedef enum {
    DIR_PLUSX,
    DIR_MINUSX,
    DIR_PLUSY,
    DIR_MINUSY,
    DIR_PLUSZ,
    DIR_MINUSZ,
} direction_e;

#define ATLAS_LENGTH 128
#define ATLAS_HEIGHT 64
#define TEXTURE_LENGTH 16
#define TEXTURE_HEIGHT 16

extern const vertex_t blockVertices[6][6];
extern const unsigned int faceVerticesSize;

extern const float quadVertices[];
extern const unsigned int quadVerticesSize;