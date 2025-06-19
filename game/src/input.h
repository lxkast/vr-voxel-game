#ifndef INPUT_H
#define INPUT_H

#include <cglm/mat4.h>
#include "GLFW/glfw3.h"
#include "camera.h"
#include "player.h"

// TODO(): Sam can you do this as well
/**
 *
 * @param window
 * @param wireframe
 * @param vr
 */
void initialiseInput(GLFWwindow *window, void (*wireframe)(), void (*vr)());

/**
 * @brief Gets the player's input every frame and makes necessary changes/calls required functions.
 * @param window A pointer to a window
 * @param camera A pointer to a camera
 * @param player A pointer to a player
 * @param w A pointer to a world
 * @param dt The time since the last frame
 * @note This function uses polling, this means that it is better for "continuous" presses, ie holding W
 */
void processPlayerInput(GLFWwindow *window, camera_t *camera, player_t *player, world_t *w, double dt);

/**
 * @brief Gets camera inputs (either mouse or headset) and updates look direction
 * @param window A pointer to a window
 * @param camera A pointer to a camera
 */
void processCameraInput(GLFWwindow *window, camera_t *camera);

#endif
