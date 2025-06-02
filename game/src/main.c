#include <cglm/cglm.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <logging.h>
#include <stdio.h>
#include "camera.h"
#include "postprocess.h"
#include "shaderutil.h"
#include "texture.h"
#include "world.h"
#include "entity.h"
#include "player.h"

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

#define SPRINT_MULTIPLIER 1.3f
#define ACCELERATION 9.f

static double previousMouse[2];

static void processPlayerInput(GLFWwindow *window, player_t *player, world_t *w) {
    vec3 acceleration = { 0.f, -10.f, 0.f };

    const float sprintMultiplier = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? SPRINT_MULTIPLIER : 1.f ;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        acceleration[2] += ACCELERATION * sprintMultiplier;  // Forward
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        acceleration[2] -= ACCELERATION * sprintMultiplier;  // Backward
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        acceleration[0] -= ACCELERATION * sprintMultiplier;  // Left
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        acceleration[0] += ACCELERATION * sprintMultiplier;  // Right
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player->entity.grounded) {
        player->entity.velocity[1] = 5;
        player->entity.grounded = false;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        player_removeBlock(player, w);
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        player_placeBlock(player, w, BL_GRASS);
    }

    changeRUFtoXYZ(acceleration, player->entity.yaw);

    glm_vec3_copy(acceleration, player->entity.acceleration);
}

static void processCameraInput(GLFWwindow *window, camera_t *camera) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    double currentMouse[2];
    glfwGetCursorPos(window, &currentMouse[0], &currentMouse[1]);
    const float dX = (float)(currentMouse[0] - previousMouse[0]);
    const float dY = (float)(currentMouse[1] - previousMouse[1]);
    previousMouse[0] = currentMouse[0];
    previousMouse[1] = currentMouse[1];
    camera_fromMouse(camera, -dX, -dY);
}

static bool wireframeView = false;
static bool previousDownO = false;
static bool postProcessingEnabled = true;
static bool previousDownP = false;

static void processInput(GLFWwindow *window) {
    const int oKey = glfwGetKey(window, GLFW_KEY_O);
    if (oKey == GLFW_PRESS && !previousDownO) {
        wireframeView = !wireframeView;
        previousDownO = true;
    }
    if (oKey == GLFW_RELEASE) {
        previousDownO = false;
    }
    const int pKey = glfwGetKey(window, GLFW_KEY_P);
    if (pKey == GLFW_PRESS && !previousDownP) {
        postProcessingEnabled = !postProcessingEnabled;
        previousDownP = true;
    }
    if (pKey == GLFW_RELEASE) {
        previousDownP = false;
    }
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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, previousMouse, previousMouse + 1);


    LOG_INFO("Initialisation complete.");
    LOG_INFO("Using OpenGL %s", glGetString(GL_VERSION));


    /*
        Initialising shaders
    */


    GLuint program;
    BUILD_SHADER_PROGRAM(
        &program, {
            glBindAttribLocation(program, 0, "aPos");
            glBindAttribLocation(program, 1, "aTexCoord");
        }, {
            LOG_ERROR("Couldn't build shader program");
            return -1;
        },
        "shaders/chunk.vert",
        "shaders/chunk.frag"
    );

    GLuint postProcessProgram;
    BUILD_SHADER_PROGRAM(
        &postProcessProgram, {
            glBindAttribLocation(program, 0, "aPos");
            glBindAttribLocation(program, 1, "aTexCoord");
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


    /*
        Set up projection matrix
    */



        mat4 projection;
        glm_perspective(FOV_Y, (float)screenWidth / (float)screenHeight, 0.1f, 500.0f, projection);

        glUseProgram(program);

        const int projectionLocation = glGetUniformLocation(program, "projection");

        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection);

        glUseProgram(0);


    // set texture unit
    {
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program, "uTextureAtlas"), 0);

        glUseProgram(0);
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
    world_init(&world);

    unsigned int spawnLoader, cameraLoader;
    world_genChunkLoader(&world, &spawnLoader);
    world_genChunkLoader(&world, &cameraLoader);
    world_updateChunkLoader(&world, spawnLoader, GLM_VEC3_ZERO);
    world_updateChunkLoader(&world, cameraLoader, GLM_VEC3_ZERO);

    player_t player;
    player_init(&player);

    double prevTime = glfwGetTime();

    postProcess_t postProcess;
    postProcess_init(&postProcess, postProcessProgram, screenWidth, screenHeight);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        processPlayerInput(window, &player, &world);
        processCameraInput(window, &camera);
        world_doChunkLoading(&world);

        world_updateChunkLoader(&world, cameraLoader, camera.eye);

        const double currentTime = glfwGetTime();
        const double dt = currentTime - prevTime;
        prevTime = currentTime;

        processEntity(&world, &player.entity, dt);
        player_attachCamera(&player, &camera);


        glClearColor(135.f/255.f, 206.f/255.f, 235.f/255.f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);


        glUseProgram(program);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program, "uTextureAtlas"), 0);
        const int modelLocation = glGetUniformLocation(program, "model");
        glPolygonMode(GL_FRONT_AND_BACK, wireframeView ? GL_LINE : GL_FILL);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection);
        const int typesLocation = glGetUniformLocation(program, "uTypes");
        glUniform1i(typesLocation, 3);

        if (postProcessingEnabled) {
            postProcess_bindBuffer(&postProcess.leftFramebuffer);
            camera_translateX(&camera, -EYE_OFFSET);
        }
        glEnable(GL_DEPTH_TEST);
        glClearColor(135.f/255.f, 206.f/255.f, 235.f/255.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera_setView(&camera, program);
        world_draw(&world, modelLocation);

        if (postProcessingEnabled) {
            postProcess_bindBuffer(&postProcess.rightFramebuffer);
            glEnable(GL_DEPTH_TEST);
            glClearColor(135.f/255.f, 206.f/255.f, 235.f/255.f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            camera_translateX(&camera, 2 * EYE_OFFSET);
        }

        camera_setView(&camera, program);
        world_draw(&world, modelLocation);

        if (postProcessingEnabled) {
            postProcess_draw(&postProcess);
            camera_translateX(&camera, -EYE_OFFSET);
        }

        glUseProgram(0);


        glfwPollEvents();
        glfwSwapBuffers(window);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            LOG_ERROR("OpenGL error: %d", err);
        }
        LOG_DEBUG("FPS: %f", 1/(dt));
    }

    world_free(&world);

    return 0;
}
