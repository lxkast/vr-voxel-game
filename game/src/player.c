#include "player.h"

void player_init(player_t *p) {
    p->cameraPitch = 0.f;
    *p = (player_t) {
        .entity = {
            .position = {0.f, 15.f, 0.f},
            .velocity = {0.f, 0.f, 0.f},
            .size = {0.6f, 1.8f, 0.6f},
            .acceleration = {0.f, 0.f, 0.f},
            .grounded = false,
            .yaw = 0,
        },
        .cameraPitch = 0.f,
        .cameraOffset = {0.3f, 1.6f, 0.3f}
    };
}

void player_attachCamera(player_t *p, camera_t *camera) {
    vec3 camPos;
    glm_vec3_add(p->entity.position, p->cameraOffset, camPos);
    camera_setPos(camera, camPos);

    vec3 BlockPosition;
    vec3 sub1 = {0.f,1.f,0.f};
    glm_vec3_sub(p->entity.position, sub1, BlockPosition);
    glm_vec3_floor(BlockPosition,BlockPosition);

    const float qx = camera->ori[0];
    const float qy = camera->ori[1];
    const float qz = camera->ori[2];
    const float qw = camera->ori[3];

    const float sinyCosp = 2.0f * ( qw*qy + qx*qz );
    const float cosyCosp = 1.0f - 2.0f * ( qy*qy + qz*qz );
    const float yaw = atan2f(sinyCosp , cosyCosp);

    p->entity.yaw = yaw;
}
