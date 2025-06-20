#ifndef HUD_H
#define HUD_H

#include <cglm/mat4.h>
#include "player.h"

/**
 * @brief Initialise buffers, shaders, required for HUD rendering
 */
void hud_init(void);

/**
 * @brief Render the HUD
 * @param perspective The perspective matrix
 * @param offset The current camera offset from its true position
 * @param camera A pointer to the camera
 * @param player A pointer to the player
 * @param textureAtlas The block texture atlas
 */
void hud_render(mat4 perspective, vec3 offset, camera_t *camera, const player_t *player, GLuint textureAtlas);

/**
 * @brief Open the HUD
 * @param camera A pointer to the camera
 * @param player A pointer to the player
 */
void open_hud(camera_t *camera, const player_t *player);

/**
 * @brief Close the HUD
 * @param camera A pointer to the camera
 * @param player A pointer to the player
 */
void close_hud(camera_t *camera, player_t *player);

#endif