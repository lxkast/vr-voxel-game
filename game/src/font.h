#ifndef FONT_H
#define FONT_H

#include <cglm/mat4.h>
#include <glad/gl.h>

typedef struct {
    GLuint texture;
} font_t;

//TODO(): Sam can you check these?

/**
 * @brief Loads a font from a file
 * @param font A pointer to a font
 * @param fontPath The file name to get the font from
 */
void fontInit(font_t *font, const char *fontPath);

/**
 * @brief Renders the font on the screen
 * @param font A pointer to a font
 * @param data The data to display
 * @param projview
 * @param rotation The rotation of the font
 * @param colour The colour of the fon
 */
void fontRender(const font_t *font, const char *data, mat4 projview, mat4 rotation, vec4 colour);

#endif
