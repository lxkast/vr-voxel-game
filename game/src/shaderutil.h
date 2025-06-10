#pragma once

#include <glad/gl.h>

/*
    A module to abstract some of the stages of shader creation.

    Example use:

    ```
    GLuint program;
    BUILD_SHADER_PROGRAM(
        &program, {
            LOG_DEBUG("Pre link");
        }, {
            LOG_ERROR("Failed to create shader program");
            return -1;
        },
        "shaders/basic.vert",
        "shaders/basic.frag"
    );
    ```

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

#define _VA_COUNT_IMPL_( \
    _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, _11, _12, _13, _14, \
    _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, \
    _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, \
    _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, \
    _57, _58, _59, _60, _61, _62, _63, _64,  N, ...)  N

#define _VA_COUNT(...) \
    _VA_COUNT_IMPL_(__VA_ARGS__, \
        64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, \
        50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, \
        36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, \
        22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10,  9, \
        8,  7,  6,  5,  4,  3,  2,  1,  0)

/*
    programPtr: Ptr to (uninitialised) GLuint.
    preLink: Block of code to run pre-linking
    fail: Block of code to run on fail
    ...: Filenames of shaders.
*/
#define BUILD_SHADER_PROGRAM(programPtr, preLink, fail, ...) \
    do { \
        int success = 1; \
        if (su_createProgramFromFilenames(programPtr, _VA_COUNT(__VA_ARGS__), __VA_ARGS__) != 0) { \
            success = 0; \
        } \
        if (success) preLink; \
        if (success && su_linkProgram(*programPtr) != 0) { \
            success = 0; \
        } \
        if (!success) fail; \
    } while(0)