#include "entity.h"
#include "logging.h"

#define MAX_RAYCAST_DISTANCE 6.f
#define RAYCAST_STEP_MAGNITUDE 0.1f
#define vec3_SIZE 12

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

/**
 * @brief Handles a collision of an entity and a block along a specific axis. Tries to
 *        resolve collisions by updating deltaP so the entity never moves inside a block.
 * @param entity The entity that is colliding
 * @param aabb The entity's bounding box
 * @param block The block we are checking for collisions with
 * @param deltaP The amount we were originally planning on moving the entity by
 * @param axisNum The axis we are resolving on
 */
static void handleAxisCollision(entity_t *entity, const aabb_t aabb, const block_bounding_t block, vec3 deltaP, const int axisNum) {
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
 * @brief Converts a list of block_data_t to block_bounding_t
 * @param buf the array containing the block_data_t blocks
 * @param numBlocks the number of blocks in the array
 * @param result the array in which to store the block_bounding_t blocks
 */
static void blockDataToBlockBounding(const block_data_t *buf, const unsigned int numBlocks, block_bounding_t *result) {
    vec3 blockSize = {1.f, 1.f, 1.f};
    for (int i = 0; i < numBlocks; i++) {
        const block_data_t block = buf[i];

        vec3 position = {(float)block.x, (float)block.y, (float)block.z};

        result[i] = (block_bounding_t){.data = block, .aabb = makeAABB(position, blockSize)};
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
    block_data_t buf[numBlocks];

    world_getBlocksInRange(w, minPoint, maxPoint, buf);

    // converting them to block_bounding_t boxes
    block_bounding_t blocks[numBlocks];

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

void updateVelocity(entity_t *entity, vec3 deltaV) {
    glm_vec3_add(entity->velocity, deltaV, entity->velocity);

    entity->velocity[0] = clamp(entity->velocity[0], -MAX_ABS_X_VELOCITY, MAX_ABS_X_VELOCITY);
    entity->velocity[1] = clamp(entity->velocity[1], -MAX_ABS_Y_VELOCITY, MAX_ABS_Y_VELOCITY);
    entity->velocity[2] = clamp(entity->velocity[2], -MAX_ABS_Z_VELOCITY, MAX_ABS_Z_VELOCITY);
}

void changeRUFtoXYZ(vec3 directionVector, const float yaw) {
    const float right = directionVector[0];
    const float forward = directionVector[2];

    directionVector[0] = right * cosf(yaw) - forward * sinf(yaw);     // X
    directionVector[2] = -right * sinf(yaw) - forward * cosf(yaw);    // Z
}

void getViewDirection(const player_t *player, vec3 out) {
    const float XZscaling = cosf(player->cameraPitch);
    out[0] = -sinf(player->entity.yaw) * XZscaling;
    out[1] = sinf(player->cameraPitch);
    out[2] = -cosf(player->entity.yaw) * XZscaling;
}

/**
 * @brief Gets the type of a block at a chosen position
 * @param w a pointer to the world
 * @param position the position to get a block at
 * @return The type of the block
 */
static block_t getBlockType(world_t *w, vec3 position) {
    block_data_t bd;
    world_getBlock(w, position, &bd);
    return bd.type;
}

/**
 * @brief Gets the block at a chosen position, with the block_bounding_t datatype.
 * @param w a pointer to the world
 * @param position the position to get a block at
 * @return The block_bounding_t block
 */
static block_bounding_t getBlockBounding(world_t *w, vec3 position) {
    block_data_t bd;
    world_getBlock(w, position, &bd);
    const aabb_t aabb = makeAABB((vec3){(float)bd.x, (float)bd.y, (float)bd.z}, (vec3){1.f, 1.f, 1.f});
    return (block_bounding_t){bd, aabb};
}

raycast_t raycast(world_t *w, const vec3 eyePosition, const vec3 viewDirection) {
    for (float i = 0; i < MAX_RAYCAST_DISTANCE; i += RAYCAST_STEP_MAGNITUDE) {
        const vec3 newPos = {
            eyePosition[0] + i * viewDirection[0],
            eyePosition[1] + i * viewDirection[1],
            eyePosition[2] + i * viewDirection[2],
        };

        const vec3 flooredPos = {floorf(newPos[0]), floorf(newPos[1]), floorf(newPos[2])};

        const block_t block_type = getBlockType(w, flooredPos);
        if (block_type != BL_AIR) {
            return (raycast_t){
                .blockPosition = {flooredPos[0], flooredPos[1], flooredPos[2]},
                .found = true
            };
        }
    }

    return (raycast_t){
        {0, 0, 0},
        false
    };
}

void processEntity(world_t *w, entity_t *entity, const float dt) {
    vec3 deltaV;
    glm_vec3_scale(entity->acceleration, dt, deltaV);
    updateVelocity(entity, deltaV);

    vec3 deltaP;
    glm_vec3_scale(entity->velocity, dt, deltaP);

    moveEntity(w, entity, deltaP);
}

raycast_t raycastDDA(world_t *w, vec3 eyePosition, vec3 viewDirection) {
    vec3 viewNormalised;
    glm_vec3_copy(viewDirection, viewNormalised);
    glm_normalize(viewNormalised);

    vec3 currentBlock;
    glm_vec3_floor(eyePosition, currentBlock);

    // stores the amount we must move along the ray to get to the next edge
    // in each direction
    vec3 oneBlockMoveDist;

    // ignoring divide by 0 error for the moment
    oneBlockMoveDist[0] = 1 / viewNormalised[0];
    oneBlockMoveDist[1] = 1 / viewNormalised[1];
    oneBlockMoveDist[2] = 1 / viewNormalised[2];

    // Calculating initial distances to next block
    vec3 nextBlockDists;

    nextBlockDists[0] = viewNormalised[0] < 0 ? eyePosition[0] - currentBlock[0] : currentBlock[0] + 1 - eyePosition[0];
    nextBlockDists[1] = viewNormalised[1] < 0 ? eyePosition[1] - currentBlock[1] : currentBlock[1] + 1 - eyePosition[1];
    nextBlockDists[2] = viewNormalised[2] < 0 ? eyePosition[2] - currentBlock[2] : currentBlock[2] + 1 - eyePosition[2];

    nextBlockDists[0] *= oneBlockMoveDist[0];
    nextBlockDists[1] *= oneBlockMoveDist[1];
    nextBlockDists[2] *= oneBlockMoveDist[2];

    float totalDistance = 0;

    while (totalDistance < MAX_RAYCAST_DISTANCE) {
        if (getBlockType(w, currentBlock) != BL_AIR) {
            return (raycast_t){
                .blockPosition = {currentBlock[0], currentBlock[1], currentBlock[2]},
                .found = true
            };
        }

        // Moving to the nearest new block
        // TODO: Finish later
    }

    // This is here so CLion will typecheck correctly
    return (raycast_t){
        .blockPosition = {0, 0, 0},
        .found = false
    };
}