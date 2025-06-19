#ifndef HUD_H
#define HUD_H

#include "player.h"
#include <cglm/mat4.h>

void hud_init(void);
void hud_render(mat4 perspective, vec3 offset, camera_t *camera, const player_t *player, GLuint textureAtlas);
void open_hud(camera_t *camera, const player_t *player);
void close_hud(camera_t *camera, player_t *player);

#endif