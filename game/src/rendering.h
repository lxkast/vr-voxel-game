#ifndef RENDERING_H
#define RENDERING_H

#include <stdbool.h>
#include "world.h"


void update_projection(const bool postProcessingEnabled, const float fov, const int screenWidth, const int screenHeight, const float renderDistance);
void rendering_init(const int screen_width, const int screen_height);
void rendering_render(world_t *world, camera_t *camera, player_t *player, bool wireframeView, bool postProcessing);




#endif //RENDERING_H
