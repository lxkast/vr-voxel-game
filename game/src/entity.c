#include "entity.h"

bool intersectsX(const aabb_t box1, const aabb_t box2) {
    return box1.min[0] < box2.max[0] && box1.max[0] >= box2.min[0];
}

bool intersectsY(const aabb_t box1, const aabb_t box2) {
    return box1.min[1] < box2.max[1] && box1.max[1] >= box2.min[1];
}

bool intersectsZ(const aabb_t box1, const aabb_t box2) {
    return box1.min[2] < box2.max[2] && box1.max[2] >= box2.min[2];
}

bool intersects(const aabb_t box1, const aabb_t box2) {
    return intersectsX(box1,box2) && intersectsY(box1,box2) && intersectsZ(box1,box2);
}

/**
 * @brief Calculates and returns a bounding box for an entity
 * @param entity The entity whose bounding box we want
 * @return The entity's bounding box
 */
aabb_t makeAABB(const entity_t *entity) {
    aabb_t box;
    glm_vec3_sub(entity->position, entity->size, box.min);
    glm_vec3_add(entity->position, entity->size, box.max);
    return box;
}

// this is TEMPORARY, will be updated when merging onto main branch
typedef enum {
    AIR,
    DIRT,
    STONE,
} block_type_e;
typedef struct {
    vec3 position;
    block_type_e type;
    aabb_t aabb;
} block_t;

// Not yet implemented - will get all adjacent blocks once that is implemented
extern block_t* getAdjacentBlocks(vec3 position, vec3 size, int *numBlocks);

/**
 * @brief Handles a collision of an entity and a block along a specific axis. Tries to
 *        resolve collisions by updating deltaP so the entity never moves inside a block.
 * @param entity The entity that is colliding
 * @param aabb The entity's bounding box
 * @param block The block we are checking for collisions with
 * @param deltaP The amount we were originally planning on moving the entity by
 * @param axisNum The axis we are resolving on
 */
void handleAxisCollision(entity_t *entity, const aabb_t aabb, const block_t block, vec3 deltaP, const int axisNum) {
    if (aabb.min[axisNum] + deltaP[axisNum] < block.aabb.max[axisNum] && aabb.max[axisNum] + deltaP[axisNum] >= block.aabb.min[axisNum]) {
        if (deltaP[axisNum] < 0) {
            deltaP[axisNum] = block.aabb.max[axisNum] - entity->position[axisNum];
            if (axisNum == 1) {
                entity->grounded = true;
            }
        } else {
            deltaP[axisNum] = block.aabb.min[axisNum] - entity->size[axisNum] - entity->position[axisNum];
        }
        entity->velocity[axisNum] = 0;
    }
}

/**
 * @brief updates the entity's position using deltaP, whilst checking for possible collisions
 * @param entity the entity whose position you're changing
 * @param deltaP the amount you want to change it by
 */
void moveEntity(entity_t *entity, vec3 deltaP) {
    const aabb_t aabb = makeAABB(entity);

    int numBlocks = 0;

    const block_t* blocks = getAdjacentBlocks(entity->position, entity->size, &numBlocks);

    // resolves collisions in Y-axis
    for (int i = 0; i < numBlocks; i++) {
        if (blocks[i].type == AIR) { continue; }
        if (intersectsX(aabb, blocks[i].aabb) && intersectsZ(aabb, blocks[i].aabb)) {
            handleAxisCollision(entity, aabb, blocks[i], deltaP, 1);
        }
    }
    // resolves collisions in X-axis
    for (int i = 0; i < numBlocks; i++) {
        if (blocks[i].type == AIR) { continue; }
        if (intersectsY(aabb, blocks[i].aabb) && intersectsZ(aabb, blocks[i].aabb)) {
            handleAxisCollision(entity, aabb, blocks[i], deltaP, 0);
        }
    }
    // resolves collisions in Z-axis
    for (int i = 0; i < numBlocks; i++) {
        if (blocks[i].type == AIR) { continue; }
        if (intersectsX(aabb, blocks[i].aabb) && intersectsY(aabb, blocks[i].aabb)) {
            handleAxisCollision(entity, aabb, blocks[i], deltaP, 2);
        }
    }

    glm_vec3_add(entity->position, deltaP, entity->position);
}

/**
 * @brief Clamps a value between a minimum and maximum value
 */
static float clamp(const float value, const float min, const float max) {
    return (value < min) ? min : (value > max) ? max : value;
}

/**
 * @brief Updates an entity's velocity
 * @param entity The entity whose velocity we are updating
 * @param deltaV The amount we want to update that velocity by
 */
void updateVelocity(entity_t *entity, vec3 deltaV) {
    glm_vec3_add(entity->velocity, deltaV, entity->velocity);

    entity->velocity[0] = clamp(entity->velocity[0], -MAX_ABS_X_VELOCITY, MAX_ABS_X_VELOCITY);
    entity->velocity[1] = clamp(entity->velocity[1], -MAX_ABS_Y_VELOCITY, MAX_ABS_Y_VELOCITY);
    entity->velocity[2] = clamp(entity->velocity[2], -MAX_ABS_Y_VELOCITY, MAX_ABS_Y_VELOCITY);
}

/**
 * @brief calculates the coordinates of the chunk a vector in 3D space is in
 * @param position The position of the item you want to get the chunk of
 * @param result The array in which the coordinates should be stored
 * @return the index of the chunk the item is in
 */
int *getChunkCoords(vec3 position, int result[3]) {
    result[0] = floor(position[0] / 16.f);
    result[1] = floor(position[1] / 16.f);
    result[2] = floor(position[2] / 16.f);
    return result;
}