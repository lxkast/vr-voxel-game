#ifndef LIGHTING_H
#define LIGHTING_H

#include "chunk.h"
#include "vertices.h"
#include "world.h"

#define LIGHT_MAX_VALUE 15
#define LIGHT_TORCH_MASK 0x0F
#define LIGHT_SUN_MASK 0xF0
#define EXTRACT_SUN(light)   (((light) & LIGHT_SUN_MASK) >> 4)
#define EXTRACT_TORCH(light) ((light) & LIGHT_TORCH_MASK)

float computeVertexLight(world_t *w, const chunk_t *c, int vx, int vy, int vz, direction_e dir);
void chunk_processLightInsertion(chunk_t *c, world_t *w);
void chunk_processLightDeletion(chunk_t *c, world_t *w);

#endif