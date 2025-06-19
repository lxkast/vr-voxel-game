#include <cglm/cglm.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <logging.h>
#include <stdio.h>
#include "input.h"
#include "camera.h"
#include "entity.h"
#include "player.h"
#include "world.h"
#include "hud.h"

#ifdef BUILD_FOR_RPI
#include "hardware/orientation.h"
#endif

#define SPRINT_MULTIPLIER 1.3f
#define GROUND_ACCELERATION 35.f
#define AIR_ACCELERATION 10.f

#define STRUCTURE_BUILD_TOOL

static int joystickID = -1;

static void (*toggle_wireframe)();
static void (*toggle_vr)();

static vec3 bottomLeft = {0.f, 0.f, 0.f};
static vec3 topRight = {0.f,0.f,0.f};
static vec3 origin = {0.f,0.f,0.f};
static block_t originBlock = BL_AIR;

static bool hudOpen = false;

/*
 * This function uses polling, this means that it is better for "continuous" presses, ie holding W
 */
void processPlayerInput(GLFWwindow *window, camera_t *camera, player_t *player, world_t *w, const double dt) {
    vec3 acceleration = { 0.f, GRAVITY_ACCELERATION, 0.f };

    const float sprintMultiplier = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? SPRINT_MULTIPLIER : 1.f;

    vec3 direction = {0.0f, 0.0f, 0.0f};

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) direction[2] += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) direction[2] -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) direction[0] -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) direction[0] += 1.0f;

    if (joystickID == -1) {
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !hudOpen) {
            open_hud(camera, player);
            hudOpen = true;
        } else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE && hudOpen) {
            close_hud(camera, player);
            hudOpen = false;
        }
    }

    glm_vec3_normalize(direction);

    const float accelVal = player->entity.grounded ? GROUND_ACCELERATION : AIR_ACCELERATION;
    acceleration[0] += direction[0] * accelVal * sprintMultiplier;
    acceleration[2] += direction[2] * accelVal * sprintMultiplier;


    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player->entity.grounded) {
        player->entity.velocity[1] = 5;
        player->entity.grounded = false;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        player_mineBlock(player, w, dt);
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        player_placeBlock(player, w);
    }


    for (char i = 0; i <= 8; i++) {
        if (glfwGetKey(window, GLFW_KEY_1 + i) == GLFW_PRESS) {
            player->hotbar.currentSlotIndex = i;
            player->hotbar.currentSlot = &(player->hotbar.slots[i]);
            player_printHotbar(player);
            break;
        }
    }

    if (joystickID != -1) {
        int axisCount;
        const float* axes = glfwGetJoystickAxes(joystickID, &axisCount);

        int buttonCount;
        const unsigned char *buttons = glfwGetJoystickButtons(joystickID, &buttonCount);
        if (buttonCount != 5 || axisCount != 2) {
            LOG_DEBUG("Joystick is not our controller, has been swapped out, buttons: %d, axis: %d", buttonCount, axisCount);
            joystickID = -1;
        } else {
            const float joySprintMultiplier = buttons[3] ? SPRINT_MULTIPLIER : 1.f;
            acceleration[2] += -axes[1] * (player->entity.grounded ? GROUND_ACCELERATION : AIR_ACCELERATION) * joySprintMultiplier;
            acceleration[0] += axes[0] * (player->entity.grounded ? GROUND_ACCELERATION : AIR_ACCELERATION) * joySprintMultiplier;

            if (buttons[4] && player->entity.grounded) {
                player->entity.velocity[1] = 5;
                player->entity.grounded = false;
            }
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            player_mineBlock(player, w, dt);
        } else {
            player->currMiningTime = 0;
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
                player_placeBlock(player, w);
            }
        }

        if (buttons[0] && !hudOpen) {
            open_hud(camera, player);
            hudOpen = true;
        } else if (!buttons[0] && hudOpen) {
            close_hud(camera, player);
            hudOpen = false;
        }
    }

    // This is temporary and for development purposes - allows us to create a structure from
    // something we have built in-game, rather than having to try to visualise it logically
    #ifdef STRUCTURE_BUILD_TOOL
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        vec3 startPos;
        vec3 lookDirection;
        glm_vec3_scale(player->lookVector, -1.f, lookDirection);
        glm_vec3_add(player->entity.position, player->cameraOffset, startPos);
        raycast_t raycast = world_raycast(w, startPos, lookDirection, 10);
        glm_vec3_copy(raycast.blockPosition, bottomLeft);
        LOG_DEBUG("FOUND BLOCK: %f, %f, %f", bottomLeft[0], bottomLeft[1], bottomLeft[2]);
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        vec3 startPos;
        vec3 lookDirection;
        glm_vec3_scale(player->lookVector, -1.f, lookDirection);
        glm_vec3_add(player->entity.position, player->cameraOffset, startPos);
        raycast_t raycast = world_raycast(w, startPos, lookDirection, 10);
        glm_vec3_copy(raycast.blockPosition, topRight);
        LOG_DEBUG("FOUND BLOCK: %f, %f, %f", topRight[0], topRight[1], topRight[2]);
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        vec3 startPos;
        vec3 lookDirection;
        glm_vec3_scale(player->lookVector, -1.f, lookDirection);
        glm_vec3_add(player->entity.position, player->cameraOffset, startPos);
        raycast_t raycast = world_raycast(w, startPos, lookDirection, 10);
        glm_vec3_copy(raycast.blockPosition, origin);
        origin[1]++;
        originBlock = getBlockType(w, raycast.blockPosition);
        LOG_DEBUG("FOUND BLOCK: %f, %f, %f", origin[0], origin[1], origin[2]);
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        vec3 minPoint;
        vec3 maxPoint;
        for (int i = 0; i < 3; i++) {
            if (bottomLeft[i] < topRight[i]) {
                minPoint[i] = bottomLeft[i];
                maxPoint[i] = topRight[i] + 1;
            } else {
                minPoint[i] = topRight[i];
                maxPoint[i] = bottomLeft[i] + 1;
            }
        }
        const int numBlocks = (int)(maxPoint[0] - minPoint[0]) *
                          (int)(maxPoint[1] - minPoint[1]) *
                          (int)(maxPoint[2] - minPoint[2]);

        // getting all the blocks in the range
        blockData_t buf[numBlocks];

        world_getBlocksInRange(w, minPoint, maxPoint, buf);

        printf("static const structureBlock_t generatedPattern[] = {\n");

        for (int i = 0; i < numBlocks; i++) {
            const blockData_t block = buf[i];
            if (block.type!= BL_AIR) {
                printf("    {%d, %d, %d, %d, 1.f, false},\n", block.type, block.x - (int)origin[0], block.y - (int)origin[1], block.z - (int)origin[2]);
            }
        }

        printf("};\n\n");

        printf("const structure_t generatedStructure = {\n");
        printf("    .numBlocks = STRUCTURE_SIZE(generatedPattern),\n");
        printf("    .blocks = generatedPattern,\n};\n\n");
    }
    #endif


    changeRUFtoXYZ(acceleration, player->entity.yaw);

    glm_vec3_copy(acceleration, player->entity.acceleration);
}

void joystickEvent(const int jid, const int event) {
    if (event == GLFW_CONNECTED) {

        int axisCount;
        glfwGetJoystickAxes(jid, &axisCount);

        int buttonCount;
        glfwGetJoystickButtons(jid, &buttonCount);
        if (axisCount == 2 && buttonCount == 5) {
            LOG_INFO("Joystick connected, id: %d", jid);
            joystickID = jid;
        }
    } else if (event == GLFW_DISCONNECTED) {
        if (jid == joystickID) {
            joystickID = -1;
        }
    }
}

static double previousMouse[2];
void processCameraInput(GLFWwindow *window, camera_t *camera) {
    #ifdef BUILD_FOR_RPI
        quaternion orientation;
        imu_getOrientation(orientation);
        camera->ori[0] = orientation[1];
        camera->ori[1] = orientation[2];
        camera->ori[2] = orientation[3];
        camera->ori[3] = orientation[0];
        camera_update(camera);
    #else
        double currentMouse[2];
        glfwGetCursorPos(window, &currentMouse[0], &currentMouse[1]);
        const float dX = (float)(currentMouse[0] - previousMouse[0]);
        const float dY = (float)(currentMouse[1] - previousMouse[1]);
        previousMouse[0] = currentMouse[0];
        previousMouse[1] = currentMouse[1];
        camera_fromMouse(camera, -dX, -dY);
    #endif
}

/*
 * This function uses a callback meaning that it is much better for instantaneous presses, e.g. switching wireframe mode
 */
static void keyCallback(GLFWwindow *window, const int key, int scancode, const int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        toggle_wireframe();
    } else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        toggle_vr();
    }
}

void initialiseInput(GLFWwindow *window, void (*wireframe)(), void (*vr)()) {
    toggle_wireframe = wireframe;
    toggle_vr = vr;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, previousMouse, previousMouse + 1);
    glfwSetJoystickCallback(joystickEvent);
    glfwSetKeyCallback(window, keyCallback);

    #ifdef BUILD_FOR_RPI
        startOrientationThread();
    #endif

    for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; i++) {
        if (glfwJoystickPresent(i)) {
            joystickEvent(i, GLFW_CONNECTED);
        }
    }
}
