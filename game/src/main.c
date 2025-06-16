#include <cglm/cglm.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <logging.h>
#include <stdio.h>
#include "analytics.h"
#include "camera.h"
#include "entity.h"
#include "player.h"
#include "postprocess.h"
#include "shaderutil.h"
#include "texture.h"
#include "world.h"

#include "input.h"

#if defined(__APPLE__) && defined(__MACH__)
#define MINOR_VERSION 2
#else
#define MINOR_VERSION 1
#endif

#define EYE_OFFSET 0.032f
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600
#define FOV_Y 45.0f
#define USING_RASPBERRY_PI false

#define GET_PROJECTION  mat4 projection; \
                        glm_perspective(FOV_Y, (float)SCREEN_WIDTH / (float)((postProcessingEnabled ? 2 : 1) * SCREEN_HEIGHT), 0.1f, 16.f * (CHUNK_LOAD_RADIUS + 1), projection);

static bool postProcessingEnabled = true;
static bool wireframeView = false;

void set_projection(int mainProjectionLocation) {
    GET_PROJECTION
    glUniformMatrix4fv(mainProjectionLocation, 1, GL_FALSE, projection);
}

void toggle_wireframeView() {
    wireframeView = !wireframeView;
}

void toggle_postprocessing() {
    postProcessingEnabled = !postProcessingEnabled;
}

int main(void) {
    /*
        Initialisation
    */

    log_init(stdout);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    //const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

    GLFWwindow *window = glfwCreateWindow((int)(screenWidth), (int)(screenHeight), "Hello, Window!", USING_RASPBERRY_PI ? primaryMonitor : NULL, NULL);

    if (window == NULL) {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress)) {
        LOG_ERROR("Failed to initialise glad");
        glfwTerminate();
        return -1;
    }

    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
    }

    glEnable(GL_DEPTH_TEST);
    initialiseInput(window, toggle_wireframeView, toggle_postprocessing);


    LOG_INFO("Initialisation complete.");
    LOG_INFO("Using OpenGL %s", glGetString(GL_VERSION));


    /*
        Initialising shaders
    */


    GLuint program;
    BUILD_SHADER_PROGRAM(
        &program, {
            glBindAttribLocation(program, 0, "aPos");
            glBindAttribLocation(program, 1, "aTexIndex");
        }, {
            LOG_ERROR("Couldn't build shader program");
            return -1;
        },
        "shaders/chunk.vert",
        "shaders/chunk.frag"
    );

    GLuint blockEntityProgram;
    BUILD_SHADER_PROGRAM(
        &blockEntityProgram, {
            glBindAttribLocation(blockEntityProgram, 0, "aPos");
            glBindAttribLocation(blockEntityProgram, 1, "aTexCoord");
        }, {
            LOG_ERROR("Couldn't build shader program");
            return -1;
        },
        "shaders/blockEntity.vert",
        "shaders/blockEntity.frag"
    );

    GLuint postProcessProgram;
    BUILD_SHADER_PROGRAM(
        &postProcessProgram, {
            glBindAttribLocation(postProcessProgram, 0, "aPos");
            glBindAttribLocation(postProcessProgram, 1, "aTexCoord");
        }, {
            LOG_ERROR("Couldn't build shader program");
            return -1;
        },
        "shaders/postprocess.vert",
        "shaders/postprocess.frag"
    );


    /*
        Textures
    */


    const GLuint texture = loadTextureRGBA("textures/atlas.png", GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);

    glUseProgram(program);
    const int mainModelLocation = glGetUniformLocation(program, "model");
    const int mainProjectionLocation = glGetUniformLocation(program, "projection");

    set_projection(mainProjectionLocation);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(program, "uTextureAtlas"), 0);

    glUseProgram(0);

    glUseProgram(blockEntityProgram);
    const int blockEntityModelLocation = glGetUniformLocation(blockEntityProgram, "model");
    const int blockEntityProjectionLocation = glGetUniformLocation(blockEntityProgram, "projection");

    set_projection(mainProjectionLocation);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(blockEntityProgram, "uTextureAtlas"), 0);

    glUseProgram(0);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        LOG_ERROR("OpenGL error: %d", err);
    }


    /*
        Main loop
    */


    // Camera setup
    camera_t camera;
    camera_init(&camera);
    vec3 p = { 0.f, 15.f, 0.f };
    camera_setPos(&camera, p);

    // World setup
    world_t world;
    world_init(&world, program, 1ULL);

    unsigned int spawnLoader, cameraLoader;
    world_genChunkLoader(&world, &spawnLoader);
    world_genChunkLoader(&world, &cameraLoader);
    world_updateChunkLoader(&world, spawnLoader, GLM_VEC3_ZERO);
    world_updateChunkLoader(&world, cameraLoader, GLM_VEC3_ZERO);

    world_doChunkLoading(&world);

    player_t player;
    player_init(&world, &player);

    postProcess_t postProcess;
    postProcess_init(&postProcess, postProcessProgram, screenWidth, screenHeight);

    analytics_t analytics;
    analytics_init(&analytics);
    double fpsDisplayAcc = 0;

    glEnable(GL_CULL_FACE);
    while (!glfwWindowShouldClose(window)) {
        analytics_startFrame(&analytics);
        glUseProgram(program);
        processPlayerInput(window, &player, &world);
        processCameraInput(window, &camera);
        world_doChunkLoading(&world);

        world_updateChunkLoader(&world, cameraLoader, camera.eye);

        world_processAllEntities(&world, analytics.dt);

        player_pickUpItemsCheck(&player, &world);

        player_attachCamera(&player, &camera);


        glClearColor(135.f/255.f, 206.f/255.f, 235.f/255.f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program, "uTextureAtlas"), 0);

        glPolygonMode(GL_FRONT_AND_BACK, wireframeView ? GL_LINE : GL_FILL);
        set_projection(mainProjectionLocation);

        world_highlightFace(&world, &camera);
        if (postProcessingEnabled) {
            glViewport(0, 0, postProcess.buffer_width, postProcess.buffer_height);
            postProcess_bindBuffer(&postProcess.leftFramebuffer);
            camera_translateX(&camera, -EYE_OFFSET);
        }
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(135.f/255.f, 206.f/255.f, 235.f/255.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera_setView(&camera, program);
        GET_PROJECTION
        world_draw(&world, mainModelLocation, &camera, projection);
        world_drawHighlight(&world, mainModelLocation);
        glUseProgram(blockEntityProgram);

        set_projection(blockEntityProjectionLocation);

        camera_setView(&camera, blockEntityProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(blockEntityProgram, "uTextureAtlas"), 0);
        world_drawAllEntities(&world, blockEntityModelLocation);
        glUseProgram(program);
        if (postProcessingEnabled) {
            postProcess_bindBuffer(&postProcess.rightFramebuffer);
            glClearColor(135.f/255.f, 206.f/255.f, 235.f/255.f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            camera_translateX(&camera, 2 * EYE_OFFSET);
            camera_setView(&camera, program);
            world_draw(&world, mainModelLocation, &camera, projection);
            world_drawHighlight(&world, mainModelLocation);

            glUseProgram(blockEntityProgram);
            set_projection(blockEntityProjectionLocation);
            camera_setView(&camera, blockEntityProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glUniform1i(glGetUniformLocation(blockEntityProgram, "uTextureAtlas"), 0);
            world_drawAllEntities(&world, blockEntityModelLocation);

            {
                static int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                glViewport(0, 0, width, height);
            }
            postProcess_draw(&postProcess);
            camera_translateX(&camera, -EYE_OFFSET);
        }

        glUseProgram(0);

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
