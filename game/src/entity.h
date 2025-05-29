#pragma once
#include <stdbool.h>
#include <cglm/cglm.h>

#define ABS(x,y) ((x>y) ? (x) : (y))

#define MAX_ABS_X_VELOCITY 2.0f
#define MAX_ABS_Y_VELOCITY 5.0f
#define MAX_ABS_Z_VELOCITY 2.0f

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

void updateVelocity(entity_t *entity, vec3 impulse);
void moveEntity(entity_t *entity, const vec3 moveVec);