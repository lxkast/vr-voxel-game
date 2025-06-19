#include <cglm/cglm.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <logging.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include "analytics.h"
#include "camera.h"
#include "entity.h"
#include "player.h"
#include "postprocess.h"
#include "shaderutil.h"
#include "texture.h"
#include "world.h"

#include "input.h"
#include "hud.h"
#include "rendering.h"

#if defined(__APPLE__) && defined(__MACH__)
#define MINOR_VERSION 2
#else
#define MINOR_VERSION 1
#endif

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600
#ifdef BUILD_FOR_RPI
#define FOV_Y 1.5f
#else
#define FOV_Y 1.0177f
#endif
#define USING_RASPBERRY_PI false

#define GET_PROJECTION  mat4 projection; \

static bool postProcessingEnabled = true;
static bool wireframeView = false;

static int actual_screen_width, actual_screen_height;
static GLFWwindow *window;

void toggle_wireframeView() {
    wireframeView = !wireframeView;
}

void toggle_postprocessing() {
    postProcessingEnabled = !postProcessingEnabled;
}

void initialiseWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

    window = glfwCreateWindow((int)(screenWidth), (int)(screenHeight), "Hello, Window!", USING_RASPBERRY_PI ? primaryMonitor : NULL, NULL);

    if (window == NULL) {
        LOG_FATAL("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        LOG_FATAL("Failed to initialise glad");
    }

    {
        glfwGetFramebufferSize(window, &actual_screen_width, &actual_screen_height);
        glViewport(0, 0, actual_screen_width, actual_screen_height);
    }

    initialiseInput(window, toggle_wireframeView, toggle_postprocessing);
}

struct chunkWorkerData {
    world_t *world;
};
void *chunkWorker(void *arg) {
    struct chunkWorkerData data = *(struct chunkWorkerData *)arg;

    const struct timespec ts = { .tv_sec = 1, .tv_nsec = 0 };

    while (true) {
        nanosleep(&ts, NULL);
        // LOG_DEBUG("Loading chunks.");
        world_doChunkLoading(data.world);
    }
}

int main(void) {
    /*
        Initialisation
    */

    log_init(stdout);
    initialiseWindow();
    rendering_init(actual_screen_width, actual_screen_height);

    LOG_INFO("Initialisation complete.");
    LOG_INFO("Using OpenGL %s", glGetString(GL_VERSION));


    // Camera setup
    camera_t camera;
    camera_init(&camera);
    vec3 p = { 0.f, 15.f, 0.f };
    camera_setPos(&camera, p);

    // World setup
    world_t world;
    world_init(&world, 40);

    unsigned int spawnLoader, cameraLoader;
    world_genChunkLoader(&world, &spawnLoader);
    world_genChunkLoader(&world, &cameraLoader);
    world_updateChunkLoader(&world, spawnLoader, GLM_VEC3_ZERO);
    world_updateChunkLoader(&world, cameraLoader, GLM_VEC3_ZERO);

    world_doChunkLoading(&world);

    player_t player;
    player_init(&world, &player);

    analytics_t analytics;
    analytics_init(&analytics);
    double fpsDisplayAcc = 0;

    struct chunkWorkerData thData = {
        .world = &world
    };
    pthread_t th;
    pthread_create(&th, NULL, chunkWorker, &thData);

    while (!glfwWindowShouldClose(window)) {
        analytics_startFrame(&analytics);
        processPlayerInput(window, &camera, &player, &world);
        processCameraInput(window, &camera);

        world_updateChunkLoader(&world, cameraLoader, camera.eye);

        world_processAllEntities(&world, analytics.dt);

        player_pickUpItemsCheck(&player, &world);

        player_attachCamera(&player, &camera);
        camera_update(&camera);

        update_projection(postProcessingEnabled, FOV_Y, actual_screen_width, actual_screen_height, CHUNK_LOAD_RADIUS);
        rendering_render(&world, &camera, &player, wireframeView, postProcessingEnabled);

        fpsDisplayAcc += analytics.dt;
        if (fpsDisplayAcc > 1.0) {
            LOG_INFO("%.0lf\n", analytics.fps);
            fpsDisplayAcc = 0.0;
        }

        glfwPollEvents();
        glfwSwapBuffers(window);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            LOG_ERROR("OpenGL error: %d", err);
        }
    }

    world_free(&world);

    return 0;
}
