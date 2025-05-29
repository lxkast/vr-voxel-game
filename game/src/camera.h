#pragma once

#include <glad/gl.h>

typedef struct {
    vec3 eye;
    versor ori;
    mat3 ruf;
} camera_t;

void camera_init(camera_t *c);


void camera_createView(camera_t *c, mat4 dest);


void camera_setPos(camera_t *c, vec3 p);

void camera_translate(camera_t *c, vec3 v);

void camera_translateX(camera_t *c, float dX);

void camera_translateY(camera_t *c, float dY);

void camera_translateZ(camera_t *c, float dZ);


void camera_fromMouse(camera_t *c, float dX, float dY);


void camera_setView(camera_t *c, GLuint program);

#define camera_front(c) c->ruf[2]
#define camera_up(c) c->ruf[1]
#define camera_right(c) c->ruf[0]