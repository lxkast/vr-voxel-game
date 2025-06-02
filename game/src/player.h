#pragma once

#include <cglm/cglm.h>
#include "camera.h"
#include "entity.h"

/**
 * @brief A struct containing player data.
 */
typedef struct {
    /// The player's entity.
    entity_t entity;
    /// Where the player is looking relative to the horizontal in radians
    float cameraPitch;
    /// The displacement of the camera from the player
    vec3 cameraOffset;
} player_t;

/**
 * @brief Initialises a player object.
 * @param p A pointer to a player
 */
void player_init(player_t *p);

/**
 * @brief Moves a camera to the player and sets orientation of the player.
 * @param p A pointer to a player
 * @param camera A pointer to a camera
 */
void player_attachCamera(player_t *p, camera_t *camera);
