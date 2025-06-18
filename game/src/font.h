#pragma once

#include <cglm/mat4.h>
#include <glad/gl.h>

typedef struct {
    GLuint texture;
} font_t;

void font_init(font_t *font, const char *filename);
void font_render(font_t *font, const char *data, mat4 projview, mat4 rotation, vec4 colour);