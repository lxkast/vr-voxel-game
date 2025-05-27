#include <cglm/cglm.h>
#include <math.h>

#include "camera.h"

void camera_init(camera_t *c) {
    glm_vec3_copy(GLM_VEC3_ZERO, c->eye);
    glm_quat_copy(GLM_QUAT_IDENTITY, c->ori);
}

void camera_createView(camera_t *c, mat4 dest) {
    glm_quat_look(c->eye, c->ori, dest);
}

void camera_translate(camera_t *c, vec3 v) {
    glm_vec3_add(c->eye, v, c->eye);
}

void camera_fromMouse(camera_t *c, vec2 prevMouse, vec2 currMouse) {
    vec3 v_0 = {
        prevMouse[0],
        currMouse[0],
        sqrtf(glm_max(0.0f, 1 - prevMouse[0] * prevMouse[0] - currMouse[0] * currMouse[0]))
    };
    vec3 v_1 = {
        prevMouse[1],
        currMouse[1],
        sqrtf(glm_max(0.0f, 1 - prevMouse[1] * prevMouse[1] - currMouse[1] * currMouse[1]))
    };

    vec3 axis;
    glm_cross(v_0, v_1, axis);
    glm_normalize(axis);

    const float angle = acosf(glm_dot(v_1, v_0));

    versor deltaQ;
    glm_quat(deltaQ, angle, axis[0], axis[1], axis[2]);

    glm_quat_mul(deltaQ, c->ori, c->ori);
}

void camera_setView(camera_t *c, GLuint program) {
    const int viewLocation = glGetUniformLocation(program, "view");
    mat4 view;
    camera_createView(c, view);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, view);
}