//
// Created by sam on 18/06/2025.
//

#include "rendering.h"

#include <logging.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <glad/gl.h>

#include "hud.h"
#include "postprocess.h"
#include "shaderutil.h"
#include "texture.h"
#include "vertices.h"
#include "world.h"
#include "GLFW/glfw3.h"

#define EYE_OFFSET 0.032f

static GLuint chunkShader;
static GLuint blockEntityShader;
static GLuint postProcessShader;
static int chunkShaderModelLocation;
static int chunkShaderProjectionLocation;
static int blockEntityShaderModelLocation;
static int blockEntityShaderProjectionLocation;

static GLuint blockAtlasTexture;
static mat4 projection;

static postProcess_t postProcess;
static int width, height;

void update_projection(const bool postProcessingEnabled, const float fov, const int screenWidth, const int screenHeight, const float renderDistance) {
    width = screenWidth;
    height = screenHeight;
    glm_perspective(fov, (float)screenWidth / (float)((postProcessingEnabled ? 2 : 1) * screenHeight), 0.1f, 16.f * (renderDistance + 1), projection);
}

static void shader_init(void) {
    BUILD_SHADER_PROGRAM(
        &chunkShader, {
            glBindAttribLocation(chunkShader, 0, "aPos");
            glBindAttribLocation(chunkShader, 1, "aTexIndex");
            glBindAttribLocation(chunkShader, 2, "aLightValue");
        }, {
            LOG_FATAL("Couldn't build chunk shader program");
        },
        "shaders/chunk.vert",
        "shaders/chunk.frag"
    );

    BUILD_SHADER_PROGRAM(
        &blockEntityShader, {
            glBindAttribLocation(blockEntityShader, 0, "aPos");
            glBindAttribLocation(blockEntityShader, 1, "aTexCoord");
        }, {
            LOG_FATAL("Couldn't build block entity shader program");
        },
        "shaders/blockEntity.vert",
        "shaders/blockEntity.frag"
    );

    BUILD_SHADER_PROGRAM(
        &postProcessShader, {
            glBindAttribLocation(postProcessShader, 0, "aPos");
            glBindAttribLocation(postProcessShader, 1, "aTexCoord");
        }, {
            LOG_FATAL("Couldn't build post processing shader program");
        },
        "shaders/postprocess.vert",
        "shaders/postprocess.frag"
    );

    blockAtlasTexture = loadTexture("textures/atlas.png", GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);

    glUseProgram(chunkShader);
    chunkShaderModelLocation = glGetUniformLocation(chunkShader, "model");
    chunkShaderProjectionLocation = glGetUniformLocation(chunkShader, "projection");
    glUseProgram(0);


    glUseProgram(blockEntityShader);
    blockEntityShaderModelLocation = glGetUniformLocation(blockEntityShader, "model");
    blockEntityShaderProjectionLocation = glGetUniformLocation(blockEntityShader, "projection");
    glUseProgram(0);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        LOG_ERROR("OpenGL error: %d", err);
    }
}

static void fog_init() {
    glUseProgram(chunkShader);
    glUniform1f(glGetUniformLocation(chunkShader, "fogStart"), FOG_START);
    glUniform1f(glGetUniformLocation(chunkShader, "fogEnd"), FOG_END);
    glUseProgram(0);
}

void rendering_init(const int screen_width, const int screen_height) {
    shader_init();

    fog_init();

    postProcess_init(&postProcess, postProcessShader, screen_width, screen_height);


    hud_init();


    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

static void render_world(world_t *world, camera_t *camera) {
    glUseProgram(chunkShader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blockAtlasTexture);
    glUniform1i(glGetUniformLocation(chunkShader, "uTextureAtlas"), 0);
    glUniformMatrix4fv(glGetUniformLocation(chunkShader, "projection"), 1, false, projection);
    glUniform1f(glGetUniformLocation(chunkShader, "fogStart"), FOG_START);
    glUniform1f(glGetUniformLocation(chunkShader, "fogEnd"), FOG_END);

    world_highlightFace(world, camera);
    camera_setView(camera, chunkShader);

    world_remeshChunks(world);

    world_draw(world, chunkShaderModelLocation, camera, projection);
    world_drawHighlight(world, chunkShaderModelLocation);

    glUseProgram(0);

    glUseProgram(blockEntityShader);
    glUniformMatrix4fv(glGetUniformLocation(blockEntityShader, "projection"), 1, false, projection);
    camera_setView(camera, blockEntityShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blockAtlasTexture);
    glUniform1i(glGetUniformLocation(blockEntityShader, "uTextureAtlas"), 0);
    world_drawAllEntities(world, blockEntityShaderModelLocation);
    glUseProgram(0);
}


static void render_with_postprocessing(world_t *world, camera_t *camera, player_t *player) {
    glViewport(0, 0, postProcess.buffer_width, postProcess.buffer_height);
    postProcess_bindBuffer(&postProcess.leftFramebuffer);
    glClearColor(135.f/255.f, 206.f/255.f, 235.f/255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera_translateX(camera, -EYE_OFFSET);
    render_world(world, camera);
    vec3 offset = {-EYE_OFFSET, 0.f, 0.f};
    hud_render(projection, offset, camera, player, blockAtlasTexture);

    postProcess_bindBuffer(&postProcess.rightFramebuffer);
    glClearColor(135.f/255.f, 206.f/255.f, 235.f/255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera_translateX(camera, 2*EYE_OFFSET);
    render_world(world, camera);

    offset[0] = EYE_OFFSET;
    hud_render(projection, offset, camera, player, blockAtlasTexture);

    glViewport(0, 0, width, height);
    postProcess_draw(&postProcess);
    camera_translateX(camera, -EYE_OFFSET);
}

static void render_without_postprocessing(world_t *world, camera_t *camera, player_t *player) {
    glViewport(0, 0, width, height);

    render_world(world, camera);
    vec3 offset = {0.f, 0.f, 0.f};
    hud_render(projection, offset, camera, player, blockAtlasTexture);
}

void rendering_render(world_t *world, camera_t *camera, player_t *player, bool wireframeView, bool postProcessing) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(135.f/255.f, 206.f/255.f, 235.f/255.f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, wireframeView ? GL_LINE : GL_FILL);
    if (postProcessing) {
        render_with_postprocessing(world, camera, player);
    } else {
        render_without_postprocessing(world, camera, player);
    }

}