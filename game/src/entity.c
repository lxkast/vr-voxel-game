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


void moveEntity(entity_t *entity, const vec3 moveVec) {
    const aabb_t aabb = makeAABB(entity);

    // Not yet implemented - will get all adjacent blocks once that is implemented
    const block_t blocks[];
    // assuming there are 12

    for (int i = 0; i < 12; i++) {
        if (blocks[i].type == AIR) { continue; }
        if (intersectsX(aabb, blocks[i].aabb) && intersectsZ(aabb, blocks[i].aabb)) {
            if (aabb.min[1] + moveVec[1] < blocks[i].aabb.max[1] && aabb.max[1] + moveVec[1] >= blocks[i].aabb.min[1]) {
                if (moveVec[1] < 0) {
                    entity->position[1] = blocks[i].aabb.max[1];
                    entity->grounded = true;
                } else {
                    entity->position[1] = blocks[i].aabb.min[1] - entity->size[1];
                }
                entity->velocity[1] = 0;
            }
        }
    }
    for (int i = 0; i < 12; i++) {
        if (blocks[i].type == AIR) { continue; }
        if (intersectsY(aabb, blocks[i].aabb) && intersectsZ(aabb, blocks[i].aabb)) {
            if (aabb.min[0] + moveVec[0] < blocks[i].aabb.max[0] && aabb.max[0] + moveVec[0] >= blocks[i].aabb.min[0]) {
                if (moveVec[0] < 0) {
                    entity->position[0] = blocks[i].aabb.max[0];
                } else {
                    entity->position[0] = blocks[i].aabb.min[0] - entity->size[0];
                }
                entity->velocity[0] = 0;
            }
        }
    }
    for (int i = 0; i < 12; i++) {
        if (blocks[i].type == AIR) { continue; }
        if (intersectsX(aabb, blocks[i].aabb) && intersectsY(aabb, blocks[i].aabb)) {
            if (aabb.min[2] + moveVec[2] < blocks[i].aabb.max[2] && aabb.max[2] + moveVec[2] >= blocks[i].aabb.min[2]) {
                if (moveVec[2] < 0) {
                    entity->position[2] = blocks[i].aabb.max[0];
                } else {
                    entity->position[2] = blocks[i].aabb.min[2] - entity->size[2];
                }
                entity->velocity[2] = 0;
            }
        }
    }
}

static float clamp(const float value, const float min, const float max) {
    return (value < min) ? min : (value > max) ? max : value;
}

void updateVelocity(entity_t *entity, vec3 impulse) {
    glm_vec3_add(entity->velocity, impulse, entity->velocity);

    entity->velocity[0] = clamp(entity->velocity[0], -MAX_ABS_X_VELOCITY, MAX_ABS_X_VELOCITY);
    entity->velocity[1] = clamp(entity->velocity[1], -MAX_ABS_Y_VELOCITY, MAX_ABS_Y_VELOCITY);
    entity->velocity[2] = clamp(entity->velocity[2], -MAX_ABS_Y_VELOCITY, MAX_ABS_Y_VELOCITY);
}
