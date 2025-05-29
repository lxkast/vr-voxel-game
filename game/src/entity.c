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

void moveEntity(const entity_t *entity, const vec3 moveVec) {
    const aabb_t aabb = makeAABB(entity);

    // Not yet implemented - will get all adjacent blocks once that is implemented
    const aabb_t blocks[];
    // assuming there are 12

    for (int i = 0; i < 12; i++) {
        if (intersectsX(aabb, blocks[i]) && intersectsZ(aabb, blocks[i])) {
            if (aabb.min[1] + moveVec[1] < blocks[i].max[1] && aabb.max[1] + moveVec[1] >= blocks[i].min[1]) {
                // TODO: handle Y-axis collision
            }
        }
    }
    for (int i = 0; i < 12; i++) {
        if (intersectsY(aabb, blocks[i]) && intersectsZ(aabb, blocks[i])) {
            if (aabb.min[0] + moveVec[0] < blocks[i].max[0] && aabb.max[0] + moveVec[0] >= blocks[i].min[0]) {
                // TODO: handle X-axis collision
            }
        }
    }
    for (int i = 0; i < 12; i++) {
        if (intersectsX(aabb, blocks[i]) && intersectsY(aabb, blocks[i])) {
            if (aabb.min[2] + moveVec[2] < blocks[i].max[2] && aabb.max[2] + moveVec[2] >= blocks[i].min[2]) {
                // TODO: handle Z-axis collision
            }
        }
    }

}