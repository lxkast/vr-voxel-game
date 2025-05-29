#include <stdio.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <logging.h>
#include <cglm/cglm.h>

#include "camera.h"
#include "shaderutil.h"
#include "vertices.h"
#include "texture.h"

#if defined(__APPLE__) && defined(__MACH__)
#define MINOR_VERSION 2
#else
#define MINOR_VERSION 1
#endif

static double previousMouse[2];

void processCameraInput(GLFWwindow *window, camera_t *camera) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_translateZ(camera, -0.01f);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_translateZ(camera, 0.01f);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_translateX(camera, -0.01f);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_translateX(camera, 0.01f);
    }

    double currentMouse[2];
    glfwGetCursorPos(window, &currentMouse[0], &currentMouse[1]);
    const float dX = (float)(currentMouse[0] - previousMouse[0]);
    const float dY = (float)(currentMouse[1] - previousMouse[1]);
    previousMouse[0] = currentMouse[0];
    previousMouse[1] = currentMouse[1];
    camera_fromMouse(camera, -dX, -dY);
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

    GLFWwindow *window = glfwCreateWindow(640, 480, "Hello, Window!", NULL, NULL);
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
        "shaders/basic.vert",
        "shaders/basic.frag"
    );


    /*
        Cube
    */


    GLuint vao;
    {
        GLuint vbo, ebo;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, grassVerticesSize, grassVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    /*
        Textures
    */
    GLuint texture = loadTextureRGBA("../../textures/textures.png", GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);

    /*
        Add basic camera setup
    */


    {
        mat4 model, view, projection;

        glm_mat4_copy(GLM_MAT4_IDENTITY, model);

        vec3 eye = { 2.5f, 2.0f, 2.5f };
        glm_lookat(eye, GLM_VEC3_ZERO, GLM_YUP, view);

        glm_perspective_default(640.0f / 480.0f, projection);

        glUseProgram(program);

        const int modelLocation = glGetUniformLocation(program, "model");
        const int viewLocation = glGetUniformLocation(program, "view");
        const int projectionLocation = glGetUniformLocation(program, "projection");

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model);
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, view);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection);

        glUseProgram(0);
    }

    // set texture unit
    {
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program, "uTextureAtlas"), 0);
    }

    /*
        Main loop
    */

    // Camera setup
    camera_t camera;
    camera_init(&camera);
    vec3 p = { 0.f, 0.f, -5.f };
    camera_setPos(&camera, p);


    while (!glfwWindowShouldClose(window)) {
        processCameraInput(window, &camera);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);


        glUseProgram(program);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        camera_setView(&camera, program);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glUseProgram(0);

        glfwPollEvents();
        glfwSwapBuffers(window);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            LOG_ERROR("OpenGL error: %d", err);
        }
    }

    return 0;
}
