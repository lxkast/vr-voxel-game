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

void handleAxisCollision(entity_t *entity, const aabb_t aabb, const block_t *blocks, vec3 deltaP, const int index, const int axisNum) {
    if (aabb.min[axisNum] + deltaP[axisNum] < blocks[index].aabb.max[axisNum] && aabb.max[axisNum] + deltaP[axisNum] >= blocks[index].aabb.min[axisNum]) {
        if (deltaP[axisNum] < 0) {
            deltaP[axisNum] = blocks[index].aabb.max[axisNum] - entity->position[axisNum];
            entity->grounded = true;
        } else {
            deltaP[axisNum] = blocks[index].aabb.min[axisNum] - entity->size[axisNum] - entity->position[axisNum];
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
            handleAxisCollision(entity, aabb, blocks, deltaP, i, 1);
        }
    }
    // resolves collisions in X-axis
    for (int i = 0; i < numBlocks; i++) {
        if (blocks[i].type == AIR) { continue; }
        if (intersectsY(aabb, blocks[i].aabb) && intersectsZ(aabb, blocks[i].aabb)) {
            handleAxisCollision(entity, aabb, blocks, deltaP, i, 0);
        }
    }
    // resolves collisions in Z-axis
    for (int i = 0; i < numBlocks; i++) {
        if (blocks[i].type == AIR) { continue; }
        if (intersectsX(aabb, blocks[i].aabb) && intersectsY(aabb, blocks[i].aabb)) {
            handleAxisCollision(entity, aabb, blocks, deltaP, i, 2);
        }
    }

    glm_vec3_add(entity->position, entity->size, entity->position);
}

static float clamp(const float value, const float min, const float max) {
    return (value < min) ? min : (value > max) ? max : value;
}

void updateVelocity(entity_t *entity, vec3 deltaV) {
    glm_vec3_add(entity->velocity, deltaV, entity->velocity);

    entity->velocity[0] = clamp(entity->velocity[0], -MAX_ABS_X_VELOCITY, MAX_ABS_X_VELOCITY);
    entity->velocity[1] = clamp(entity->velocity[1], -MAX_ABS_Y_VELOCITY, MAX_ABS_Y_VELOCITY);
    entity->velocity[2] = clamp(entity->velocity[2], -MAX_ABS_Y_VELOCITY, MAX_ABS_Y_VELOCITY);
}
