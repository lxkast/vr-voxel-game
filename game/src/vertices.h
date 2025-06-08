#pragma once

typedef struct {
    float x,y,z;
    int texIndex;
} vertex_t;

extern const float cubeVertices[];
extern const unsigned int cubeVerticesSize;

extern const unsigned int cubeIndices[];
extern const unsigned int cubeIndicesSize;

extern const vertex_t frontFaceVertices[];
extern const vertex_t backFaceVertices[];
extern const vertex_t topFaceVertices[];
extern const vertex_t bottomFaceVertices[];
extern const vertex_t leftFaceVertices[];
extern const vertex_t rightFaceVertices[];
extern const unsigned int faceVerticesSize;

extern const float quadVertices[];

extern const unsigned int quadVerticesSize;

extern const unsigned int quadIndices[];

extern const unsigned int quadIndicesSize;