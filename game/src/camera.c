#include <cglm/cglm.h>
#include <math.h>
#include <logging.h>
#include "hardware/orientation.h"

#include "camera.h"
/**
 * @brief Calculates and sets the ruf matrix based on the orientation.
 * @param c A pointer to a camera object
 */
static void camera_setRuf(camera_t *c) {
    glm_quat_mat3(c->ori, c->ruf);
}

void camera_init(camera_t *c) {
    glm_vec3_copy(GLM_VEC3_ZERO, c->eye);
    glm_quat_for(GLM_ZUP, GLM_YUP, c->ori);
    camera_setRuf(c);
}

void camera_update(camera_t *c) {
    quaternion orientation;
    imu_getOrientation(orientation);
    c->ori[0] = orientation[1];
    c->ori[1] = orientation[2];
    c->ori[2] = orientation[3];
    c->ori[3] = orientation[0];

    camera_setRuf(c);
}

void camera_createView(camera_t *c, mat4 dest) {
    glm_quat_look(c->eye, c->ori, dest);
}

void camera_setPos(camera_t *c, vec3 p) {
    glm_vec3_copy(p, c->eye);
}

void camera_translate(camera_t *c, vec3 v) {
    vec3 delta;
    glm_mat3_mulv(c->ruf, v, delta);
    glm_vec3_add(c->eye, delta, c->eye);
}

void camera_translateX(camera_t *c, const float dX) {
    glm_vec3_muladds(camera_right(c), dX, c->eye);
}

void camera_translateY(camera_t *c, const float dY) {
    glm_vec3_muladds(camera_up(c), dY, c->eye);
}

void camera_translateZ(camera_t *c, const float dZ) {
    glm_vec3_muladds(camera_front(c), dZ, c->eye);
}

void camera_fromMouse(camera_t *c, const float dX, const float dY) {
    return;
    versor qYaw;
    glm_quat(qYaw, dX * 0.01f, 0.0f, 1.0f, 0.0f);
    glm_quat_mul(qYaw, c->ori, c->ori);

    vec3 right;
    glm_quat_rotatev(c->ori, GLM_XUP, right);
    glm_vec3_normalize(right);

    versor qPitch;
    glm_quat(qPitch, dY * 0.01f, right[0], right[1], right[2]);
    glm_quat_mul(qPitch, c->ori, c->ori);

    glm_quat_normalize(c->ori);
    camera_setRuf(c);
}

void camera_setView(camera_t *c, GLuint program) {
    const int viewLocation = glGetUniformLocation(program, "view");
    mat4 view;
    camera_createView(c, view);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, view);
}
