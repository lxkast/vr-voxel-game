#pragma once

#include <glad/gl.h>

/*
    A module to abstract some of the stages of shader creation.

    Example use:

    ```
        GLuint programHandle;
        if (
            su_createProgramFromFilenames(&programHandle, 2,
                "shaders/basic.vert",
                "shaders/basic.frag"
            ) != 0
        ) {
            LOG_ERROR("Failed to create shader program");
            return -1;
        }
        if (su_linkProgram(programHandle) != 0) {
            LOG_ERROR("Failed to link shader program");
            return -1;
        }
    ```
*/

/*
    SU_VERTEX: Vertex shader
    SU_FRAGMENT: Fragment shader
    SU_DETERMINE: Will try and determine from filename
*/
typedef enum {
    SU_VERTEX,
    SU_FRAGMENT,
    SU_DETERMINE
} su_shader_t;

/*
    Creates and compiles a shader corresponding to the filename.
    The shader will try and compile to the kind corresponding to type.
*/
int su_initialiseShader(GLuint *shader, const char *fileName, su_shader_t type);

/*
    Creates a shader program given an array of size n of shader objects.
*/
int su_createProgramFromHandles(GLuint *program, int n, const GLuint *shaderHandles);

/*
    Creates a shader program based of varargs of filenames.
    Automatically determines the type for each shader.
*/
int su_createProgramFromFilenames(GLuint *program, int n, ...);

/*
    Links the shader program.
*/
int su_linkProgram(GLuint program);