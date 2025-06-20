#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <logging.h>
#include <string.h>
#include "shaderutil.h"

static int loadShaderSource(const GLuint shader, const char *fileName) {
    FILE *fp = fopen(fileName, "rb");
    if (!fp) {
        LOG_ERROR("Error opening file %s", fileName);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    const size_t fileSize = ftell(fp);
    rewind(fp);

    char *buf = malloc(sizeof(char) * (fileSize + 1));
    if (!buf) {
        LOG_ERROR("Error allocating memory");
        fclose(fp);
        return -1;
    }

    const size_t bytesRead = fread(buf, sizeof(char), fileSize, fp);
    if (bytesRead != fileSize) {
        LOG_ERROR("Error reading file %s", fileName);
        fclose(fp);
        free(buf);
        return -1;
    }
    fclose(fp);

    buf[fileSize] = '\0';

    glShaderSource(shader, 1, (const GLchar * const *)&buf, NULL);

    free(buf);

    return 0;
}

static GLenum findShaderType(const char *fileName) {
    char *extension = strrchr(fileName, '.');
    if (!extension) return -1;
    if (strcmp(extension, ".vert") == 0) {
        return GL_VERTEX_SHADER;
    }
    if (strcmp(extension, ".frag") == 0) {
        return GL_FRAGMENT_SHADER;
    }
    return GL_INVALID_ENUM;
}

int su_initialiseShader(GLuint *shader, const char *fileName, su_shader_t type) {
    GLenum realType;
    switch (type) {
        case SU_VERTEX:
            realType = GL_VERTEX_SHADER;
            break;
        case SU_FRAGMENT:
            realType = GL_FRAGMENT_SHADER;
            break;
        case SU_DETERMINE:
            if ((realType = findShaderType(fileName)) != GL_INVALID_ENUM) {
                break;
            }
        default:
            LOG_ERROR("Failed to understand shader type");
            return -1;
    }

    int success;
    char infoLog[512];

    const GLuint shaderHandle = glCreateShader(realType);


    if (loadShaderSource(shaderHandle, fileName) != 0) {
        LOG_ERROR("Error loading shader %s", fileName);
        return -1;
    }
    glCompileShader(shaderHandle);
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderHandle, 512, NULL, infoLog);
        LOG_ERROR("Error compiling shader %s:\n%s", fileName, infoLog);
        return -1;
    }
    *shader = shaderHandle;
    return 0;
}

int su_createProgramFromHandles(GLuint *program, const int n, const GLuint *shaderHandles) {
    if (!shaderHandles) {
        LOG_ERROR("Error creating shader program");
        return -1;
    }

    const GLuint programHandle = glCreateProgram();

    for (int i = 0; i < n; i++) {
        glAttachShader(programHandle, shaderHandles[i]);
    }

    *program = programHandle;
    return 0;
}

int su_createProgramFromFilenames(GLuint *program, const int n, ...) {
    GLuint *shaderHandles = malloc(sizeof(GLuint) * n);

    va_list args;

    va_start(args, n);
    for (int i = 0; i < n; i++) {
        const char *filename = va_arg(args, char *);
        if (su_initialiseShader(shaderHandles + i, filename, SU_DETERMINE) != 0) {
            LOG_ERROR("Failed to initialise shader no. %d", i);
            free(shaderHandles);
            return -1;
        }
    }
    va_end(args);

    if (su_createProgramFromHandles(program, n, shaderHandles) != 0) {
        free(shaderHandles);
        return -1;
    }

    free(shaderHandles);

    return 0;
}

int su_linkProgram(const GLuint program) {
    int success;

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
        LOG_ERROR("Failed to link shader program:\n%s", infoLog);
        return -1;
    }

    return 0;
}