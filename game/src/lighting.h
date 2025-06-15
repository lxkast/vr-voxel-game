#pragma once
#include "chunk.h"
#include "vertices.h"

#define LIGHT_MAX_VALUE 15

float computeVertexLight(chunk_t *c, int vx, int vy, int vz, direction_e dir);
void chunk_processLighting(chunk_t *c);