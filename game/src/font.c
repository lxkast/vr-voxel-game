#include <logging.h>
#include <string.h>
#include <cglm/mat4.h>
#include "font.h"
#include "shaderutil.h"
#include "texture.h"
#include "vertices.h"

GLuint charVAO;
GLuint charVBO;


static GLuint fontProgram;

#define ROWS 16
#define COLS 16

/**
 * @brief Initialises a mash for a character
 */
static void initCharMesh(void) {
    glGenVertexArrays(1, &charVAO);
    glGenBuffers(1, &charVBO);
    glBindBuffer(GL_ARRAY_BUFFER, charVAO);
    glBindVertexArray(charVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    float *mesh = malloc(squareVerticesSize);
    memcpy(mesh, squareVertices, squareVerticesSize);
    for (int i = 0; i < 6; i++) {
        mesh[5*i + 3] /= (float) ROWS;
        mesh[5*i + 4] /= (float) COLS;
    }
    glBufferData(GL_ARRAY_BUFFER, squareVerticesSize, mesh, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void font_init(font_t *font, const char *fontPath) {
    const GLuint texture = loadTexture(fontPath, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
    font->texture = texture;
    initCharMesh();

    BUILD_SHADER_PROGRAM(
        &fontProgram, {
            glBindAttribLocation(fontProgram, 0, "aPos");
            glBindAttribLocation(fontProgram, 1, "aTexCoord");
        }, {
            LOG_ERROR("Couldn't build shader program");
            return;
        },
        "shaders/font.vert",
        "shaders/font.frag"
    );
}

/**
 *
 * @param font A pointer to a font
 * @param character The character to render
 * @param projview
 * @param model
 * @param colour The colour of the character
 */
static void renderChar(const font_t *font, const char character, mat4 projview, mat4 model, vec4 colour) {
    glUseProgram(fontProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->texture);
    glUniform1i(glGetUniformLocation(fontProgram, "uTextureAtlas"), 0);

    glUniformMatrix4fv(glGetUniformLocation(fontProgram, "model"), 1, GL_FALSE, (const GLfloat*)model);
    glUniformMatrix4fv(glGetUniformLocation(fontProgram, "projection"), 1, GL_FALSE, (const GLfloat*)projview);

    const int row = ROWS - 1 - (character / ROWS);
    const int col = character % ROWS;

    glUniform2f(glGetUniformLocation(fontProgram, "texOffset"),  (float) col / COLS, (float) row / ROWS);
    glUniform4fv(glGetUniformLocation(fontProgram, "textColour"), 1, colour);
    glBindVertexArray(charVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void fontRender(const font_t *font, const char *data, mat4 projview, mat4 rotation, vec4 colour) {
    vec3 character = {2, 0, 0};

    mat4 transPerCharM;
    glm_translate_make(transPerCharM, character);

    mat4 everything;
    glm_mat4_copy(projview, everything);

    while (*data != '\0') {
        renderChar(font, data[0], everything, rotation, colour);
        glm_mat4_mul(everything, transPerCharM, everything);
        data++;
    }
}