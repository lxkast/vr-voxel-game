#ifndef ENTITY_H
#define ENTITY_H

#include <cglm/cglm.h>
#include <stdbool.h>
#include "block.h"

typedef struct world_t world_t;

/// A struct holding information on entity
typedef struct entity_t {
    /// The entity's position
    vec3 position;
    /// The entity's velocity
    vec3 velocity;
    /// The entity's size
    vec3 size;
    /// The entity's acceleration
    vec3 acceleration;
    /// Whether the entity is grounded (not in the air)
    bool grounded;
    /// The yaw of the entity
    float yaw;
} entity_t;

/// A struct storing the min and max points of an AABB bounding box
typedef struct {
    /// The min corner
    vec3 min;
    /// The max corner
    vec3 max;
} aabb_t;

typedef struct {
    /// The block
    blockData_t data;
    /// The block's bounding box
    aabb_t aabb;
} blockBounding_t;

extern void world_getBlocksInRange(world_t *w, vec3 minPoint, const vec3 maxPoint, blockData_t buf[]);

extern bool world_getBlock(world_t *w, const vec3 position, blockData_t *bd);

/**
 * @brief Determines if two bounding boxes intersect in the X-axis
 * @param entity the entity to check collisions with
 * @param blockPosition the position of the block to check collisions with
 * @return Whether box1 and box2 intersect in the X-axis
 */
bool intersectsWithBlock(entity_t entity, ivec3 blockPosition);

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
void processEntity(world_t *w, entity_t *entity, double dt);

/**
 * @brief Performs the ceil function on a vec3
 * @param v the vector to apply the function to
 * @param dest the vector to store the result in
 * @note This was based on the existing glm_vec3_floor function
 */
void glm_vec3_ceil(vec3 v, vec3 dest);

/**
 * @brief Checks if two entities intersect
 * @param entity1 The first entity
 * @param entity2 The second entity
 * @return Whether they intersect
 */
bool entitiesIntersect(entity_t entity1, entity_t entity2);

#endif