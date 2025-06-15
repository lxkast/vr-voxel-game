#pragma once
#include "chunk.h"
#include "vertices.h"

#define LIGHT_MAX_VALUE 15
#define LIGHT_TORCH_MASK 0x0F
#define LIGHT_SUN_MASK 0xF0

float computeVertexLight(chunk_t *c, int vx, int vy, int vz, direction_e dir);
void chunk_processLighting(chunk_t *c);