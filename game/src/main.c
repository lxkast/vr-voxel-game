#include <stdio.h>
#ifdef USE_GL
#include <glad/gl.h>
#elifdef USE_GLES
#include <glad/gles2.h>
#endif
#include <GLFW/glfw3.h>
#include <logging.h>

int main(void) {
    log_init(stdout);

    char *err;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(640, 480, "Hello, Window!", NULL, NULL);
    printf("%d", glfwGetError(&err));
    printf("%s", err);
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

    LOG_INFO("Initialisation complete.");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}
