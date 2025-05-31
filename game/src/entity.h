#pragma once

#include <stdbool.h>
#include <cglm/cglm.h>
#include "world.h"

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
    vec3 acceleration;
    bool grounded;
    // stored in radians
    float yaw;   // rename to orientation?
} entity_t;

typedef struct {
    entity_t entity;        // for minecraft this is typically 0.6x1.8x0.6
    float cameraPitch;      // where the player is looking relative to the horizontal in radians
    vec3 cameraOffset;   // if we implement translational headset movement this will change
                            // default value of 0.3x1.6x0.3  (this isn't official just a guess)
    // member for inventory
    // member for hotbar
} player_t;

typedef struct {
    vec3 min;
    vec3 max;
} aabb_t;

typedef struct {
    vec3 blockPosition;
    bool found;
} raycast_t;

// this is TEMPORARY, will be updated when merging onto main branch
typedef enum {
    AIR,
    DIRT,
    STONE,
} block_type_e;

typedef struct {
    block_data_t data;
    aabb_t aabb;
} block_bounding_t;

void updateVelocity(entity_t *entity, vec3 deltaV);
void updateVelocityViewRel(entity_t *entity, vec3 deltaV);
void moveEntity(world_t *w, entity_t *entity, vec3 deltaP);
void updateEntity(world_t *w);
void changeRUFtoXYZ(vec3 directionVector, float yaw);
raycast_t raycast(world_t *w, const vec3 eyePosition, const vec3 viewDirection);
