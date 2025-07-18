#ifndef VERTICES_H
#define VERTICES_H

#include <cglm/cglm.h>

/// A struct containing data about a vertex
typedef struct {
    /// x,y and z coordinates
    float x,y,z;
    int texIndex;
    /// The light value of a vertex
    float lightValue;
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

#define ATLAS_LENGTH 256
#define ATLAS_HEIGHT 64
#define TEXTURE_LENGTH 16
#define TEXTURE_HEIGHT 16

extern const vertex_t blockVertices[6][6];
extern const unsigned int faceVerticesSize;

extern const float itemBlockVertices[];
extern const unsigned int itemBlockVerticesSize;

extern const float quadVertices[];
extern const unsigned int quadVerticesSize;

extern const float squareVertices[];
extern const unsigned int squareVerticesSize;

#endif
