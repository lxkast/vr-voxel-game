#pragma once

#include <cglm/cglm.h>
#include "camera.h"
#include "entity.h"

typedef struct {
    entity_t entity;      // for minecraft this is typically 0.6x1.8x0.6
    float cameraPitch;    // where the player is looking relative to the horizontal in radians
    vec3 cameraOffset;    // if we implement translational headset movement this will change
                          // default value of 0.3x1.6x0.3  (this isn't official just a guess)
    // member for inventory
    // member for hotbar
} player_t;

void player_init(player_t *p);

void player_attachCamera(player_t *p, camera_t *camera);
