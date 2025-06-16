#include "hud.h"

#include <glad/gl.h>
#include <cglm/cglm.h>
#include "block.h"
#include <logging.h>
#include "shaderutil.h"
#include "player.h"
#include "vertices.h"
#include "item.h"

#define ANGLE_BETWEEN 0.2617

static versor BLOCK_ORIENTATION = {0.354, 0.354, -0.146, 0.854};
static versor GAP_BETWEEN;

static const vec4 PANE_COLOUR = {0.8f, 0.8f, 0.8f, 1.f};
static const vec4 SELECTED_COLOUR = {0.8f, 0.8f, 0.4f, 1.f};

static GLuint itemVao;
static GLuint itemVbo;

static GLuint borderVao;
static GLuint borderVbo;

static versor initialRot;

bool shouldRender = false;

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

GLuint blockItemProgram;
GLuint borderProgram;
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
    meshItemEntity();
    initBorder();
    glm_quat_init(initialRot, 0.f, 0.f, 0.f, 1.f);
}

static void render_block_in_pane(mat4 projview, block_t blockType, vec3 centercoords, versor blockOrientation, float scale, versor frameOrientation, GLuint textureAtlas) {
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
    glm_translate_make(t, (vec3) {-0.5, -0.5, -0.5});
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

    glUniformMatrix4fv(glGetUniformLocation(blockItemProgram, "model"), 1, GL_FALSE, model);
    glUniformMatrix4fv(glGetUniformLocation(blockItemProgram, "projection"), 1, GL_FALSE, projview);
    glUniform2f(glGetUniformLocation(blockItemProgram, "texOffset"), TEXTURE_LENGTH * (float) blockType / ATLAS_LENGTH, 0);
    glBindVertexArray(itemVao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

static void render_block_pane(mat4 projview, vec3 centercoords, float scale, bool selected, versor frameOrientation) {
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
    glUniformMatrix4fv(glGetUniformLocation(borderProgram, "model"), 1, GL_FALSE, model);
    glUniformMatrix4fv(glGetUniformLocation(borderProgram, "projection"), 1, GL_FALSE, projview);
    glUniform4fv(glGetUniformLocation(borderProgram, "colour"), 1, selected ? SELECTED_COLOUR : PANE_COLOUR);
    glBindVertexArray(borderVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


/*
 * The orientation represents the orientation around the player, the transform for centercoords is carried out before
 * orientation, so orientation rotates around the player.
 */
static void render_item(mat4 projview, hotbarItem_t item, vec3 centercoords, versor orientation, bool selected, GLuint textureAtlas) {
#define SCALE 0.2f
    glClear(GL_DEPTH_BUFFER_BIT);
    render_block_pane(projview, centercoords, SCALE, selected, orientation);
    glClear(GL_DEPTH_BUFFER_BIT);
    if (item.type != NOTHING) {
        render_block_in_pane(projview, ITEM_TO_BLOCK[item.type], centercoords, BLOCK_ORIENTATION, SCALE , orientation, textureAtlas);
    }
}

static void apply_rotation_n_times(versor q, int n, versor dest) {
    for (int i = 0; i < n; i++) {
        glm_quat_mul(dest, q, dest);
    }
}

static int calculate_looking_at(versor camera) {
    vec3 right = {1, 0, 0};

    vec3 newDir;
    versor inv;
    glm_quat_inv(initialRot, inv);

    versor diff;
    glm_quat_mul(inv, camera, diff);

    glm_quat_rotatev(diff, right, newDir);

    // remove y direction
    newDir[1] = 0.f;
    double angle = atan2(newDir[2], newDir[0]);

    int lookingAt = roundf(angle / ANGLE_BETWEEN);
    if (lookingAt < 0) lookingAt = 0;
    if (lookingAt > HOTBAR_SLOTS-1) lookingAt = HOTBAR_SLOTS - 1;
    return lookingAt;
}

void hud_render(mat4 perspective, camera_t *camera, player_t *player, GLuint textureAtlas) {
    if (!shouldRender) return;
    vec3 centercoords = {0, 0, -2};
    versor orientation = {0, 0, 0, 1};

    int at = calculate_looking_at(camera->ori);

    mat4 view;
    versor inverted;
    glm_quat_inv(camera->ori, inverted);
    glm_quat_mat4(inverted, view);

    mat4 view2;
    glm_quat_mat4(initialRot, view2);

    mat4 projview;
    glm_mat4_mul(perspective, view, projview);
    glm_mat4_mul(projview, view2, projview);

    glm_quat_normalize(orientation);
    for (int i = 0; i < HOTBAR_SLOTS; i++) {
        render_item(projview, player->hotbar.slots[i], centercoords, orientation, i == at, textureAtlas);
        glm_quat_mul(orientation, GAP_BETWEEN, orientation);
    }
}

void open_hud(camera_t *camera, player_t *player) {
    shouldRender = true;
    glm_quat_copy(camera->ori, initialRot);

    versor inv;
    glm_quat_inv(GAP_BETWEEN, inv);
    apply_rotation_n_times(inv, player->hotbar.currentSlotIndex, initialRot);
}

void close_hud(camera_t *camera, player_t *player) {
    int lookingAt = calculate_looking_at(camera->ori);
    player->hotbar.currentSlotIndex = lookingAt;
    player->hotbar.currentSlot = &(player->hotbar.slots[lookingAt]);
    shouldRender = false;
}