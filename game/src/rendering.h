#ifndef RENDERING_H
#define RENDERING_H

#include <stdbool.h>
#include "world.h"

void rendering_updateProjection(bool postProcessingEnabled, float fov, int screenWidth, int screenHeight, float renderDistance);
void rendering_init(int screen_width, int screen_height);
void rendering_render(world_t *world, camera_t *camera, const player_t *player, bool wireframeView, bool postProcessing);

#endif RENDERING_H
