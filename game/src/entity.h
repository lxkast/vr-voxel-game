#pragma once
#include <stdbool.h>
#include <cglm/cglm.h>

typedef enum {
    PLAYER,
    ITEM,
} entity_type;

typedef struct {
    vec3 position;
    vec3 velocity;
    vec3 size;
    entity_type type;
    bool grounded;
} entity_t;

typedef struct {
    vec3 min;
    vec3 max;
} aabb_t;