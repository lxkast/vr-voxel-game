#include <cglm/cglm.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <logging.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <time.h>
#include "analytics.h"
#include "camera.h"
#include "entity.h"
#include "input.h"
#include "player.h"
#include "rendering.h"
#include "world.h"

#include "input.h"
#include "hud.h"

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

    window = glfwCreateWindow((int)(screenWidth), (int)(screenHeight), "Voxel Game", USING_RASPBERRY_PI ? primaryMonitor : NULL, NULL);

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
    atomic_bool run;
    atomic_bool finished;
    world_t *world;
};

void *chunkWorker(void *arg) {
    struct chunkWorkerData *data = (struct chunkWorkerData *)arg;

    const struct timespec ts = { .tv_sec = 0, .tv_nsec = 10 };

    while (atomic_load_explicit(&data->run, memory_order_acquire)) {
        nanosleep(&ts, NULL);
        world_doChunkLoading(data->world);
    }
    atomic_store_explicit(&data->finished, true, memory_order_release);

    return (void *) 0;
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
    atomic_store_explicit(&thData.run, true, memory_order_release);
    atomic_store_explicit(&thData.finished, false, memory_order_release);

    pthread_t th;
    pthread_create(&th, NULL, (void *)chunkWorker, &thData);

    while (true) {
        if (glfwWindowShouldClose(window)) {
            if (atomic_load_explicit(&thData.finished, memory_order_acquire)) {
                break;
            }
            atomic_store_explicit(&thData.run, false, memory_order_release);
        }

        glfwSwapInterval(1);

        analytics_startFrame(&analytics);
        processPlayerInput(window, &camera, &player, &world, analytics.dt);
        processCameraInput(window, &camera);

        world_updateChunkLoader(&world, cameraLoader, camera.eye);

        world_processAllEntities(&world, analytics.dt);
        main_thread_free(&world.queues.chunkBufferFreeQueue);

        player_pickUpItemsCheck(&player, &world);

        player_attachCamera(&player, &camera);
        camera_update(&camera);

        rendering_updateProjection(postProcessingEnabled, FOV_Y, actual_screen_width, actual_screen_height, CHUNK_LOAD_RADIUS);
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
