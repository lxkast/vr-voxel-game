#include "hud.h"

#include <glad/gl.h>
#include <cglm/cglm.h>
#include "block.h"
#include <logging.h>
#include <string.h>
#include "shaderutil.h"
#include "player.h"
#include "vertices.h"
#include "item.h"
#include "font.h"

#define ANGLE_BETWEEN 0.2617

static versor BLOCK_ORIENTATION = {0.354f, 0.354f, -0.146f, 0.854f};
static versor GAP_BETWEEN;

static const vec4 PANE_COLOUR = {0.8f, 0.8f, 0.8f, 1.f};
static const vec4 SELECTED_COLOUR = {0.8f, 0.8f, 0.4f, 1.f};
static vec4 TEXT_COLOUR = {0.f, 0.f, 0.f, 1.f};


static GLuint itemVao;
static GLuint itemVbo;

static GLuint borderVao;
static GLuint borderVbo;

static versor initialRot;

bool shouldRender = false;
font_t font;

/**
 * @brief Initialise an item entity mesh
 */
static void meshItemEntity(void) {
    glGenVertexArrays(1, &itemVao);
    glGenBuffers(1, &itemVbo);
    glBindBuffer(GL_ARRAY_BUFFER, itemVbo);
    glBindVertexArray(itemVao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    float *mesh = malloc(itemBlockVerticesSize);
    memcpy(mesh, itemBlockVertices, itemBlockVerticesSize);
    const block_t type = ITEM_TO_BLOCK[ITEM_LEAF];
    for (int i = 0; i < 36; ++i) {
        mesh[5 * i + 3] = TEXTURE_LENGTH * mesh[5 * i + 3] / ATLAS_LENGTH;
    }
    glBufferData(GL_ARRAY_BUFFER, itemBlockVerticesSize, mesh, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    free(mesh);
}

/**
 * @brief initialise the border mesh
 */
static void initBorder(void) {
    glGenVertexArrays(1, &borderVao);
    glGenBuffers(1, &borderVbo);
    glBindBuffer(GL_ARRAY_BUFFER, borderVao);
    glBindVertexArray(borderVao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    float *mesh = malloc(squareVerticesSize);
    memcpy(mesh, squareVertices, squareVerticesSize);
    glBufferData(GL_ARRAY_BUFFER, squareVerticesSize, mesh, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm_quat(GAP_BETWEEN, -ANGLE_BETWEEN, 0.f,1.f, 0.f);
}

static GLuint blockItemProgram;
static GLuint borderProgram;

void hud_init(void) {
    BUILD_SHADER_PROGRAM(
        &blockItemProgram, {
            glBindAttribLocation(blockItemProgram, 0, "aPos");
            glBindAttribLocation(blockItemProgram, 1, "aTexCoord");
        }, {
            LOG_ERROR("Couldn't build shader program");
            return;
        },
        "shaders/blockItem.vert",
        "shaders/blockItem.frag"
    );
    BUILD_SHADER_PROGRAM(
        &borderProgram, {
            glBindAttribLocation(blockItemProgram, 0, "aPos");
            glBindAttribLocation(blockItemProgram, 1, "aTexCoord");
        }, {
            LOG_ERROR("Couldn't build shader program");
            return;
        },
        "shaders/blockFrame.vert",
        "shaders/blockFrame.frag"
    );

    font_init(&font, "textures/font.png");
    meshItemEntity();
    initBorder();
    glm_quat_init(initialRot, 0.f, 0.f, 0.f, 1.f);
}

/**
 * @brief Render the block inside the pane
 * @param projview The projection and view matrices
 * @param blockType The current block type
 * @param centercoords The center coords of the block pane
 * @param blockOrientation The orientation of the block within the frame
 * @param scale The scale of the block within the frame
 * @param frameOrientation The orientation of the frame itself
 * @param textureAtlas The texture atlas for blocks
 */
static void renderBlockInPane(mat4 projview,
                                const block_t blockType,
                                vec3 centercoords,
                                versor blockOrientation,
                                const float scale,
                                versor frameOrientation,
                                const GLuint textureAtlas) {
    // render a block, first render single block, then orthogonal perspective, then render block itself
    glUseProgram(blockItemProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureAtlas);
    glUniform1i(glGetUniformLocation(blockItemProgram, "uTextureAtlas"), 0);

    mat4 translate;
    glm_translate_make(translate, centercoords);
    mat4 rotation;
    glm_quat_mat4(blockOrientation, rotation);

    mat4 model;
    glm_mat4_identity(model);


    mat4 t;
    glm_translate_make(t, (vec3) {-0.5f, -0.5f, -0.5f});
    glm_mat4_mul(model, t, model);


    mat4 scaleM;
    glm_mat4_identity(scaleM);
    glm_scale_uni(scaleM, scale);
    glm_mat4_mul(scaleM, model, model);


    mat4 proj2 = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0.001f, 0}, {0, 0, 0, 1}};

    glm_mat4_mul(rotation, model, model);
    glm_mat4_mul(proj2, model, model);


    mat4 frameRot;
    glm_quat_mat4(frameOrientation, frameRot);

    glm_mat4_mul(translate, model, model);
    glm_mat4_mul(frameRot, model, model); // NOTE flipped translate and rot multiply to make orientation around player

    glUniformMatrix4fv(glGetUniformLocation(blockItemProgram, "model"), 1, GL_FALSE, (const GLfloat*)model);
    glUniformMatrix4fv(glGetUniformLocation(blockItemProgram, "projection"), 1, GL_FALSE, (const GLfloat*)projview);
    glUniform2f(glGetUniformLocation(blockItemProgram, "texOffset"),
                TEXTURE_LENGTH * (float) blockType / ATLAS_LENGTH,
                0);
    glBindVertexArray(itemVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

/**
 * @brief Render the background pane
 * @param projview The projection and view matrices
 * @param centercoords The center coords of the pane
 * @param scale The scale of the pane
 * @param selected Whether the pane is currently selected or not
 * @param frameOrientation The orientation of the frame
 */
static void renderBlockPane(mat4 projview,
                            vec3 centercoords,
                            const float scale,
                            const bool selected,
                            versor frameOrientation) {
    glUseProgram(borderProgram);
    mat4 translate;
    glm_translate_make(translate, centercoords);

    mat4 model;
    glm_mat4_identity(model);
    glm_scale_uni(model, scale);

    mat4 rot;
    glm_quat_mat4(frameOrientation, rot);

    glm_mat4_mul(translate, model, model);
    glm_mat4_mul(rot, model, model); // NOTE flipped translate and rot multiply to make orientation around player
    glUniformMatrix4fv(glGetUniformLocation(borderProgram, "model"), 1, GL_FALSE, (const GLfloat*)model);
    glUniformMatrix4fv(glGetUniformLocation(borderProgram, "projection"), 1, GL_FALSE, (const GLfloat*)projview);
    glUniform4fv(glGetUniformLocation(borderProgram, "colour"), 1, selected ? SELECTED_COLOUR : PANE_COLOUR);
    glBindVertexArray(borderVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

/**
 * @brief Render a pane and item, orientation is applied after centercoords to make it easy to rotate around the player.
 * @param projview The projection and view matrices
 * @param item The hotbar item to be rendered
 * @param centercoords The center coords of the rendering, this is applied before orientation
 * @param orientation The orientation around the player, applyed after center coords
 * @param selected Whether the current frame is seelcted
 * @param textureAtlas The block texture atlas
 */
static void renderItem(mat4 projview,
                        const hotbarItem_t item,
                        vec3 centercoords,
                        versor orientation,
                        const bool selected,
                        const GLuint textureAtlas) {
#define SCALE 0.2f
    glClear(GL_DEPTH_BUFFER_BIT);
    renderBlockPane(projview, centercoords, SCALE, selected, orientation);
    glClear(GL_DEPTH_BUFFER_BIT);
    if (item.type != NOTHING) {
        renderBlockInPane(projview, ITEM_TO_BLOCK[item.type], centercoords, BLOCK_ORIENTATION, SCALE , orientation, textureAtlas);
    }

    if (item.type != NOTHING) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        mat4 rot;
        glm_quat_mat4(orientation, rot);
        mat4 translate;
        glm_translate_make(translate, centercoords);
        mat4 scaleM;
        vec3 t = {0.11f, -0.15f, 0.f};
        glm_translate_make(scaleM, t);
        glm_scale_uni(scaleM, 0.03f);
        mat4 res;
        glm_mat4_mul(translate, scaleM, res);
        glm_mat4_mul(rot, res, res);
        glm_mat4_mul(projview, res, res);
        mat4 ident;
        glm_mat4_identity(ident);

        char quantityString[5];
        snprintf(quantityString, 5, "%d", item.count);
        font_render(&font, quantityString, res, ident, TEXT_COLOUR);
        glEnable(GL_DEPTH_TEST);
    }
}

/**
 * @brief Applies the quaternion q n times to the destination quaternion.
 * @param q The quaternion
 * @param n The number of times
 * @param dest The quaternion to apply it to
 */
static void applyRotationNTimes(versor q, const int n, versor dest) {
    for (int i = 0; i < n; i++) {
        glm_quat_mul(dest, q, dest);
    }
}

/**
 * @brief Calculate which hotbar slot the player is currently looking at
 * @param camera The current camera
 * @return An index in 0..<HOTBARSLOTS representing the current hotbar slot.
 */
static int calculateLookingAt(versor camera) {
    vec3 right = {1, 0, 0};

    vec3 newDir;
    versor inv;
    glm_quat_inv(initialRot, inv);

    versor diff;
    glm_quat_mul(inv, camera, diff);

    glm_quat_rotatev(diff, right, newDir);

    // remove y direction
    newDir[1] = 0.f;
    const double angle = atan2f(newDir[2], newDir[0]);

    int lookingAt = (int)round(angle / ANGLE_BETWEEN);
    if (lookingAt < 0) lookingAt = 0;
    if (lookingAt > HOTBAR_SLOTS-1) lookingAt = HOTBAR_SLOTS - 1;
    return lookingAt;
}

void hud_render(mat4 perspective, vec3 offset, camera_t *camera, const player_t *player, const GLuint textureAtlas) {
    if (!shouldRender) return;
    vec3 centercoords = {0, 0, -2};
    versor orientation = {0, 0, 0, 1};

    const int at = calculateLookingAt(camera->ori);

    mat4 view;
    versor inverted;
    glm_quat_inv(camera->ori, inverted);
    glm_quat_mat4(inverted, view);

    mat4 translate;
    glm_translate_make(translate, offset);
    mat4 translateI;
    glm_mat4_inv(translate, translateI);
    glm_mat4_mul(translateI, view, view);

    mat4 view2;
    glm_quat_mat4(initialRot, view2);

    mat4 projview;
    glm_mat4_mul(perspective, view, projview);
    glm_mat4_mul(projview, view2, projview);

    glm_quat_normalize(orientation);
    for (int i = 0; i < HOTBAR_SLOTS; i++) {
        renderItem(projview, player->hotbar.slots[i], centercoords, orientation, i == at, textureAtlas);
        glm_quat_mul(orientation, GAP_BETWEEN, orientation);
    }
}

void open_hud(camera_t *camera, const player_t *player) {
    shouldRender = true;
    glm_quat_copy(camera->ori, initialRot);

    versor inv;
    glm_quat_inv(GAP_BETWEEN, inv);
    applyRotationNTimes(inv, player->hotbar.currentSlotIndex, initialRot);
}

void close_hud(camera_t *camera, player_t *player) {
    const int lookingAt = calculateLookingAt(camera->ori);
    player->hotbar.currentSlotIndex = (char)lookingAt;
    player->hotbar.currentSlot = &(player->hotbar.slots[lookingAt]);
    shouldRender = false;
}