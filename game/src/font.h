#pragma once

#include <cglm/mat4.h>
#include <glad/gl.h>

typedef struct {
    GLuint texture;
} font_t;

/**
 * @brief Initialise a font
 * @param font A pointer to the font to initialise
 * @param filename A string representing the filename of the font atlas
 */
void font_init(font_t *font, const char *filename);

/**
 * @brief Render a string in a given font
 * @param font A pointer to the font to render with
 * @param data A string to render
 * @param projview The projection and view matrices
 * @param rotation Any operations to be carried out on a single character
 * @param colour The colour of the text to render
 */
void font_render(font_t *font, const char *data, mat4 projview, mat4 rotation, vec4 colour);