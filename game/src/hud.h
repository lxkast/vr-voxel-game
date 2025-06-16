#pragma once
#include <cglm/mat4.h>

#include "player.h"


void hud_init(void);
void hud_render(mat4 perspective, camera_t *camera, player_t *player, GLuint textureAtlas);
void open_hud(camera_t *camera, player_t *player);
void close_hud(camera_t *camera, player_t *player);