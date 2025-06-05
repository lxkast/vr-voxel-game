#include "player.h"
#include "GLFW/glfw3.h"
#include "logging.h"

static const int faceToBlock[6][3] = {{-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1} };

static void setBlockCooldown(player_t *p) {
    p->blockCooldown = glfwGetTime() + BLOCK_COOLDOWN_TIME;
}

static bool onBlockCooldown(player_t *p) {
    return glfwGetTime() < p->blockCooldown;
}

void player_init(player_t *p) {
    *p = (player_t){
        .entity = {
                   .position = {0.f, 15.f, 0.f},
                   .velocity = {0.f, 0.f, 0.f},
                   .size = {0.6f, 1.8f, 0.6f},
                   .acceleration = {0.f, 0.f, 0.f},
                   .grounded = false,
                   .yaw = 0,
                   },
        .lookVector = { 0.f, 0.f, 0.f },
        .cameraOffset = {0.3f, 1.6f, 0.3f}
    };
}

void player_attachCamera(player_t *p, camera_t *camera) {
    vec3 camPos;
    glm_vec3_add(p->entity.position, p->cameraOffset, camPos);
    camera_setPos(camera, camPos);

    vec3 blockPosition;
    vec3 sub1 = {0.f, 1.f, 0.f};
    glm_vec3_sub(p->entity.position, sub1, blockPosition);
    glm_vec3_floor(blockPosition, blockPosition);

    const float qx = camera->ori[0];
    const float qy = camera->ori[1];
    const float qz = camera->ori[2];
    const float qw = camera->ori[3];

    const float sinyCosp = 2.0f * (qw * qy + qx * qz);
    const float cosyCosp = 1.0f - 2.0f * (qy * qy + qz * qz);
    const float yaw = atan2f(sinyCosp, cosyCosp);

    p->entity.yaw = yaw;
    glm_vec3_copy(camera->ruf[2], p->lookVector);
}

void player_removeBlock(player_t *p, world_t *w) {
    if (onBlockCooldown(p)) {
        return;
    }

    vec3 camPos;

    glm_vec3_add(p->entity.position, p->cameraOffset, camPos);

    // remove minus sign later
    vec3 lookVector;
    glm_vec3_scale(p->lookVector, -1, lookVector);

    const raycast_t raycastBlock = world_raycast(w, camPos, lookVector);

    if (raycastBlock.found) {
        world_removeBlock(w,(int)raycastBlock.blockPosition[0], (int)raycastBlock.blockPosition[1], (int)raycastBlock.blockPosition[2]);
        setBlockCooldown(p);
    }
}

void player_placeBlock(player_t *p, world_t *w, const block_t block) {
    if (onBlockCooldown(p)) {
        return;
    }
    vec3 camPos;

    glm_vec3_add(p->entity.position, p->cameraOffset, camPos);

    // remove minus sign later
    vec3 lookVector;
    glm_vec3_scale(p->lookVector, -1, lookVector);

    const raycast_t raycastBlock = world_raycast(w, camPos, lookVector);

    if (raycastBlock.found) {
        const int *moveDelta = faceToBlock[raycastBlock.face];

        ivec3 newBlockPosition;

        newBlockPosition[0] = (int)raycastBlock.blockPosition[0] - moveDelta[0];
        newBlockPosition[1] = (int)raycastBlock.blockPosition[1] - moveDelta[1];
        newBlockPosition[2] = (int)raycastBlock.blockPosition[2] - moveDelta[2];

        if (!intersectsWithBlock(p->entity, newBlockPosition)) {
            world_placeBlock(w, newBlockPosition[0], newBlockPosition[1], newBlockPosition[2], block);
            setBlockCooldown(p);
        }
   }
}