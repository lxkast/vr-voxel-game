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
 * @param position The position of the object whose bounding box we want
 * @param size The size of the object we want to get the bounding box of
 * @return The entity's bounding box
 */
aabb_t makeAABB(vec3 position, vec3 size) {
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
void handleAxisCollision(entity_t *entity, const aabb_t aabb, const block_bounding_t block, vec3 deltaP, const int axisNum) {
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

void blockDataToBlockBounding(const block_data_t *buf, const unsigned int numBlocks, block_bounding_t *result) {
    vec3 blockSize = {1.f, 1.f, 1.f};
    for (int i = 0; i < numBlocks; i++) {
        const block_data_t block = buf[i];

        vec3 position = {(float)block.x, (float)block.y, (float)block.z};

        result[i] = (block_bounding_t){.data = buf[0], .aabb = makeAABB(position, blockSize)};
    }
}

/**
 * @brief updates the entity's position using deltaP, whilst checking for possible collisions
 * @param entity the entity whose position you're changing
 * @param deltaP the amount you want to change it by
 */
void moveEntity(world_t *w, entity_t *entity, vec3 deltaP) {
    const aabb_t aabb = makeAABB(entity->position, entity->size);

    vec3 minPoint, maxPoint;
    vec3 shiftBy1 = {1.f, 1.f, 1.f};

    glm_vec3_sub(entity->position, shiftBy1, minPoint);
    glm_vec3_add(entity->position, entity->size, maxPoint);
    glm_vec3_add(entity->position, shiftBy1, maxPoint);

    const int numBlocks = (int)((maxPoint[0] - minPoint[0]) * (maxPoint[1] - minPoint[1]) * (maxPoint[2] - minPoint[2]));

    block_data_t buf[numBlocks];

    world_getBlocksInRange(w,minPoint, maxPoint, buf);

    block_bounding_t blocks[numBlocks];

    blockDataToBlockBounding(buf,numBlocks,blocks);

    // resolves collisions in Y-axis
    for (int i = 0; i < numBlocks; i++) {
        if (blocks[i].data.type == AIR) { continue; }
        if (intersectsX(aabb, blocks[i].aabb) && intersectsZ(aabb, blocks[i].aabb)) {
            handleAxisCollision(entity, aabb, blocks[i], deltaP, 1);
        }
    }
    // resolves collisions in X-axis
    for (int i = 0; i < numBlocks; i++) {
        if (blocks[i].data.type == AIR) { continue; }
        if (intersectsY(aabb, blocks[i].aabb) && intersectsZ(aabb, blocks[i].aabb)) {
            handleAxisCollision(entity, aabb, blocks[i], deltaP, 0);
        }
    }
    // resolves collisions in Z-axis
    for (int i = 0; i < numBlocks; i++) {
        if (blocks[i].data.type == AIR) { continue; }
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
 * @brief Updates an entity's velocity based on world coordinates
 * @param entity The entity whose velocity we are updating
 * @param deltaV The amount we want to update that velocity by in x,y,z
 */
void updateVelocity(entity_t *entity, vec3 deltaV) {
    glm_vec3_add(entity->velocity, deltaV, entity->velocity);

    entity->velocity[0] = clamp(entity->velocity[0], -MAX_ABS_X_VELOCITY, MAX_ABS_X_VELOCITY);
    entity->velocity[1] = clamp(entity->velocity[1], -MAX_ABS_Y_VELOCITY, MAX_ABS_Y_VELOCITY);
    entity->velocity[2] = clamp(entity->velocity[2], -MAX_ABS_Y_VELOCITY, MAX_ABS_Y_VELOCITY);
}

/**
 * @brief Updates an entity's velocity based on where it is looking.
 *        Assumes yaw = 0 implies -Z, yaw = pi/2 implies X and so on.
 * @param entity The entity whose velocity we are updating
 * @param deltaV The amount we want to update that velocity by relative to direction
 *               deltaV = {forward, up, right}
 */
void updateVelocityViewRel(entity_t *entity, vec3 deltaV) {
    entity->velocity[0] = clamp(entity->velocity[0] + deltaV[0] * sinf(entity->yaw) + deltaV[2] * cosf(entity->yaw), -MAX_ABS_X_VELOCITY, MAX_ABS_X_VELOCITY);
    entity->velocity[1] = clamp(entity->velocity[1] + deltaV[1], -MAX_ABS_Y_VELOCITY, MAX_ABS_Y_VELOCITY);
    entity->velocity[2] = clamp(entity->velocity[2] + deltaV[0] * -cosf(entity->yaw) + deltaV[2] * sinf(entity->yaw), -MAX_ABS_Z_VELOCITY, MAX_ABS_Z_VELOCITY);
}

/**
 * @brief Transforms a vector from right, upwards, forwards to x,y,z
 * @param directionVector The direction vector to change
 * @param yaw The yaw of the object
 */
void changeRUFtoXYZ(vec3 directionVector, const float yaw) {
    const float right = directionVector[0];
    const float forward = directionVector[2];

    directionVector[0] = right * sinf(yaw) + forward * cosf(yaw);   // X
    directionVector[2] = forward * -cosf(yaw) + right * sinf(yaw);  // Z
}

// Assumes yaw = 0 implies -Z, yaw = pi/2 implies X and so on.
void getViewDirection(const player_t *player, vec3 out) {
    const float XZscaling = cosf(player->cameraPitch);
    out[0] = -sinf(player->entity.yaw) * XZscaling;
    out[1] = sinf(player->cameraPitch);
    out[2] = -cosf(player->entity.yaw) * XZscaling;
}

#define MAX_RAYCAST_DISTANCE 6.f
#define RAYCAST_STEP_MAGNITUDE 0.1f
#define vec3_SIZE 12
// TODO: Will implement when chunks implemented
block_type_e getBlockType(world_t *w, vec3 position) {
    block_data_t bd;
    world_getBlock(w, position, &bd);
    return bd.type;
}

block_bounding_t getBlockBounding(world_t *w, vec3 position) {
    block_data_t bd;
    world_getBlock(w, position, &bd);
    const aabb_t aabb = makeAABB((vec3){(float)bd.x, (float)bd.y, (float)bd.z}, (vec3){1.f, 1.f , 1.f});
    return (block_bounding_t){bd, aabb};
}

// will rewrite in DDA later
raycast_t raycast(world_t *w, const vec3 eyePosition, const vec3 viewDirection) {
    for (float i = 0; i < MAX_RAYCAST_DISTANCE; i += RAYCAST_STEP_MAGNITUDE) {
        const vec3 newPos = {eyePosition[0] + i * viewDirection[0],
                       eyePosition[1] + i * viewDirection[1],
                       eyePosition[2] + i * viewDirection[2],
        };

        const vec3 flooredPos = { floorf(newPos[0]), floorf(newPos[1]), floorf(newPos[2]) };

        const block_type_e block_type = getBlockType(w, flooredPos);
        if (block_type != AIR) {
            return (raycast_t){
                .blockPosition = {flooredPos[0], flooredPos[1], flooredPos[2]},
                .found = true};
        }
    }

    return (raycast_t){{0,0,0}, false};
}

void floorVec3(const vec3 vector, vec3 result) {
    result[0] = floorf(vector[0]);
    result[1] = floorf(vector[1]);
    result[2] = floorf(vector[2]);
}

raycast_t raycastDDA(world_t *w, vec3 eyePosition, vec3 viewDirection) {
    vec3 viewNormalised;
    glm_vec3_copy(viewDirection, viewNormalised);
    glm_normalize(viewNormalised);

    vec3 currentBlock;
    glm_vec3_floor(eyePosition,currentBlock);

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
        if (getBlockType(w, currentBlock) != AIR) {
            return (raycast_t){
                .blockPosition = {currentBlock[0], currentBlock[1], currentBlock[2]},
                .found = true};
        }

        // Moving to the nearest new block
        // TODO: Finish later
    }

    // This is here so CLion will typecheck correctly
    return (raycast_t){
        .blockPosition = {0, 0, 0},
        .found = false};
}

void processEntity(world_t *w, entity_t *entity, const float dt) {
    vec3 deltaV;
    glm_vec3_scale(entity->acceleration, dt, deltaV);
    updateVelocity(entity, deltaV);

    vec3 deltaP;
    glm_vec3_scale(entity->velocity, dt, deltaP);
    moveEntity(w, entity, deltaP);
}