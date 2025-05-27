#pragma once

#include <glad/gl.h>

typedef struct {
    vec3 eye;
    versor ori;
} camera_t;

void camera_init(camera_t *c);


void camera_createView(camera_t *c, mat4 dest);


void camera_translate(camera_t *c, vec3 v);

void camera_translateX(camera_t *c, float dX);

void camera_translateY(camera_t *c, float dY);

void camera_translateZ(camera_t *c, float dZ);


void camera_fromMouse(camera_t *c, vec2 prevMouse, vec2 currMouse);


void camera_setView(camera_t *c, GLuint program);