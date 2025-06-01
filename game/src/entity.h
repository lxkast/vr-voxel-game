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

typedef struct {
    block_data_t data;
    aabb_t aabb;
} block_bounding_t;

/**
 * @brief Updates an entity's velocity based on world coordinates
 * @param entity The entity whose velocity we are updating
 * @param deltaV The amount we want to update that velocity by in x,y,z
 */
void updateVelocity(entity_t *entity, vec3 deltaV);

/**
 * @brief Transforms a vector from right, upwards, forwards to x,y,z
 * @param directionVector The direction vector to change
 * @param yaw The yaw of the object
 */
void changeRUFtoXYZ(vec3 directionVector, float yaw);

/**
 * @brief Updates an entity's data every frame.
 * @param w a pointer to the world
 * @param entity the entity to update
 * @param dt the time since the last tick
 */
void processEntity(world_t *w, entity_t *entity, float dt);

/**
 * @brief Performs the ceil function on a vec3
 * @param v the vector to apply the function to
 * @param dest the vector to store the result in
 */
void glm_vec3_ceil(vec3 v, vec3 dest);
