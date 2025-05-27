#pragma once

typedef struct {
    vec3 eye;
    versor ori;
} camera_t;

void camera_init(camera_t *c);

void camera_createView(camera_t *c, mat4 dest);

void camera_translate(camera_t *c, vec3 v);

void camera_fromMouse(camera_t *c, vec2 prevMouse, vec2 currMouse);