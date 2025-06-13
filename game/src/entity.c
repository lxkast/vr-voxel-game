#include "entity.h"

#define vec3_SIZE 12
#define VELOCITY_CUTOFF 0.05f
#define GROUND_FRICTION_CONSTANT 0.2f
#define AIR_FRICTION_CONSTANT 0.6f

/**
 * @brief Determines if two bounding boxes intersect in the X-axis
 * @param box1 the first bounding box
 * @param box2 the second bounding box
 * @return whether box1 and box2 intersect in the X-axis
 */
static bool intersectsX(const aabb_t box1, const aabb_t box2) {
    return box1.min[0] < box2.max[0] && box1.max[0] > box2.min[0];
}

/**
 * @brief Determines if two bounding boxes intersect in the Y-axis
 * @param box1 the first bounding box
 * @param box2 the second bounding box
 * @return Whether box1 and box2 intersect in the Y-axis
 */
static bool intersectsY(const aabb_t box1, const aabb_t box2) {
    return box1.min[1] < box2.max[1] && box1.max[1] > box2.min[1];
}

/**
 * @brief Determines if two bounding boxes intersect in the Z-axis
 * @param box1 the first bounding box
 * @param box2 the second bounding box
 * @return Whether box1 and box2 intersect in the Z-axis
 */
static bool intersectsZ(const aabb_t box1, const aabb_t box2) {
    return box1.min[2] < box2.max[2] && box1.max[2] > box2.min[2];
}

/**
 * @brief Determines if two bounding boxes intersect in the X-axis
 * @param box1 the first bounding box
 * @param box2 the second bounding box
 * @return Whether box1 and box2 intersect in the X-axis
 */
static bool intersects(const aabb_t box1, const aabb_t box2) {
    return intersectsX(box1, box2) && intersectsY(box1, box2) && intersectsZ(box1, box2);
}

/**
 * @brief Calculates and returns a bounding box for an entity
 * @param position The position of the object whose bounding box we want
 * @param size The size of the object we want to get the bounding box of
 * @return The entity's bounding box
 */
static aabb_t makeAABB(vec3 position, vec3 size) {
    aabb_t box;
    glm_vec3_copy(position, box.min);
    glm_vec3_add(position, size, box.max);
    return box;
}

bool intersectsWithBlock(const entity_t entity, ivec3 blockPosition) {
    const aabb_t entityAABB = makeAABB(entity.position, entity.size);
    const aabb_t blockAABB = makeAABB((vec3){(float)blockPosition[0], (float)blockPosition[1], (float)blockPosition[2]}, (vec3){1.f, 1.f, 1.f});

    return intersects(entityAABB, blockAABB);
}

/**
 * @brief Handles a collision of an entity and a block along a specific axis. Tries to
 *        resolve collisions by updating deltaP so the entity never moves inside a block.
 * @param entity The entity that is colliding
 * @param aabb The entity's bounding box
 * @param block The block we are checking for collisions with
 * @param deltaP The amount we were originally planning on moving the entity by
 * @param axisNum The axis we are resolving on
 */
static void handleAxisCollision(entity_t *entity, const aabb_t aabb, const blockBounding_t block, vec3 deltaP, const int axisNum) {
    if (deltaP[axisNum] == 0.f) {
        return;
    }
    // checks to see if it collides in specified axis
    if (aabb.min[axisNum] + deltaP[axisNum] < block.aabb.max[axisNum] && aabb.max[axisNum] + deltaP[axisNum] > block.aabb.min[axisNum]) {
        if (deltaP[axisNum] < 0) {
            // if velocity is negative, makes the entity's min point end up at the block's max point
            deltaP[axisNum] = block.aabb.max[axisNum] - aabb.min[axisNum];
            if (axisNum == 1) {
                // if the entity lands on a block from above, set grounded to true
                entity->grounded = true;
            }
        } else {
            // if velocity is positive, makes the entity's max point end up at the block's min point
            deltaP[axisNum] = block.aabb.min[axisNum] - aabb.max[axisNum];
        }
        entity->velocity[axisNum] = 0;
    }
}

/**
 * @brief Converts a list of blockData_t to block_bounding_t
 * @param buf the array containing the blockData_t blocks
 * @param numBlocks the number of blocks in the array
 * @param result the array in which to store the block_bounding_t blocks
 */
static void blockDataToBlockBounding(const blockData_t *buf, const unsigned int numBlocks, blockBounding_t *result) {
    vec3 blockSize = {1.f, 1.f, 1.f};
    for (int i = 0; i < numBlocks; i++) {
        const blockData_t block = buf[numBlocks - i - 1];

        vec3 position = {(float)block.x, (float)block.y, (float)block.z};

        result[i] = (blockBounding_t){.data = block, .aabb = makeAABB(position, blockSize)};
    }
}

void glm_vec3_ceil(vec3 v, vec3 dest) {
    dest[0] = ceilf(v[0]);
    dest[1] = ceilf(v[1]);
    dest[2] = ceilf(v[2]);
}

/**
 * @brief Updates the entity's position using deltaP, whilst checking for possible collisions
 * @param entity the entity whose position you're changing
 * @param deltaP the amount you want to change it by
 */
static void moveEntity(world_t *w, entity_t *entity, vec3 deltaP) {
    // defining the entity's AABB bounding box
    const aabb_t aabb = makeAABB(entity->position, entity->size);

    // working out the bottom left and top right corners of the cuboid of blocks
    // around the player we want to check collisions with
    vec3 minPoint, maxPoint;
    vec3 shiftBy1 = {1.f, 1.f, 1.f};

    // calculating bottom left corner
    glm_vec3_sub(entity->position, shiftBy1, minPoint);
    glm_vec3_floor(minPoint, minPoint);

    // calculating top right corner
    glm_vec3_add(entity->position, entity->size, maxPoint);
    glm_vec3_add(maxPoint, shiftBy1, maxPoint);
    glm_vec3_ceil(maxPoint, maxPoint);

    // calculating how many blocks there are
    const int numBlocks = (int)(maxPoint[0] - minPoint[0]) *
                          (int)(maxPoint[1] - minPoint[1]) *
                          (int)(maxPoint[2] - minPoint[2]);

    // getting all the blocks in the range
    blockData_t buf[numBlocks];

    world_getBlocksInRange(w, minPoint, maxPoint, buf);

    // converting them to blockBounding_t boxes
    blockBounding_t blocks[numBlocks];

    blockDataToBlockBounding(buf, numBlocks, blocks);

    // resolves collisions in Y-axis
    for (int i = 0; i < numBlocks; i++) {
        if (blocks[i].data.type == BL_AIR) {
            continue;
        }
        if (intersectsX(aabb, blocks[i].aabb) && intersectsZ(aabb, blocks[i].aabb)) {
            handleAxisCollision(entity, aabb, blocks[i], deltaP, 1);
        }
    }

    // resolves collisions in X-axis
    for (int i = 0; i < numBlocks; i++) {
        if (blocks[i].data.type == BL_AIR) {
            continue;
        }
        if (intersectsY(aabb, blocks[i].aabb) && intersectsZ(aabb, blocks[i].aabb)) {
            handleAxisCollision(entity, aabb, blocks[i], deltaP, 0);
        }
    }

    // resolves collisions in Z-axis
    for (int i = 0; i < numBlocks; i++) {
        if (blocks[i].data.type == BL_AIR) {
            continue;
        }
        if (intersectsX(aabb, blocks[i].aabb) && intersectsY(aabb, blocks[i].aabb)) {
            handleAxisCollision(entity, aabb, blocks[i], deltaP, 2);
        }
    }

    // updates position
    glm_vec3_add(entity->position, deltaP, entity->position);
}

/**
 * @brief Clamps a value between a minimum and maximum value
 * @param value The value to be clamped
 * @param min The minimum value allowed
 * @param max The maximum value allowed
 */
static float clamp(const float value, const float min, const float max) {
    return (value < min) ? min : ((value > max) ? max : value);
}

static float velocityCutoff(const float velocity) {
    return ((velocity < 0 ? -velocity : velocity) < VELOCITY_CUTOFF) ? 0 : velocity;
}

void updateVelocity(entity_t *entity, vec3 deltaV) {
    glm_vec3_add(entity->velocity, deltaV, entity->velocity);

    entity->velocity[0] = velocityCutoff(entity->velocity[0]);
    entity->velocity[1] = velocityCutoff(entity->velocity[1]);
    entity->velocity[2] = velocityCutoff(entity->velocity[2]);
}

void changeRUFtoXYZ(vec3 directionVector, const float yaw) {
    const float right = directionVector[0];
    const float forward = directionVector[2];

    directionVector[0] = right * cosf(yaw) - forward * sinf(yaw);     // X
    directionVector[2] = -right * sinf(yaw) - forward * cosf(yaw);    // Z
}

/**
 * @brief Gets the block at a chosen position, with the block_bounding_t datatype.
 * @param w a pointer to the world
 * @param position the position to get a block at
 * @return The block_bounding_t block
 */
static blockBounding_t getBlockBounding(world_t *w, vec3 position) {
    blockData_t bd;
    world_getBlock(w, position, &bd);
    const aabb_t aabb = makeAABB((vec3){(float)bd.x, (float)bd.y, (float)bd.z}, (vec3){1.f, 1.f, 1.f});
    return (blockBounding_t){bd, aabb};
}

void processEntity(world_t *w, entity_t *entity, const double dt) {
    // handling friction

    if (entity->grounded) {
        const float frictionFactor = powf(GROUND_FRICTION_CONSTANT, (float)dt / 0.25f);
        entity->velocity[0] = velocityCutoff(entity->velocity[0] * frictionFactor);
        entity->velocity[2] = velocityCutoff(entity->velocity[2] * frictionFactor);
    } else {
        const float frictionFactor = powf(AIR_FRICTION_CONSTANT, (float)dt / 0.25f);
        entity->velocity[0] = velocityCutoff(entity->velocity[0] * frictionFactor);
        entity->velocity[2] = velocityCutoff(entity->velocity[2] * frictionFactor);    }

    vec3 deltaV;
    glm_vec3_scale(entity->acceleration, (float)dt, deltaV);
    updateVelocity(entity, deltaV);

    vec3 deltaP;
    glm_vec3_scale(entity->velocity, (float)dt, deltaP);

    moveEntity(w, entity, deltaP);
}

bool entitiesIntersect(entity_t entity1, entity_t entity2) {
    const aabb_t entity1aabb = makeAABB(entity1.position, entity1.size);
    const aabb_t entity2aabb = makeAABB(entity2.position, entity1.size);
    return intersects(entity1aabb, entity2aabb);
}