#pragma once
#include <cglm/mat4.h>

#include "camera.h"
#include "player.h"
#include "GLFW/glfw3.h"

void initialiseInput(GLFWwindow *window, void (*wireframe)(), void (*vr)());
void processPlayerInput(GLFWwindow *window, player_t *player, world_t *w);
void processCameraInput(GLFWwindow *window, camera_t *camera);