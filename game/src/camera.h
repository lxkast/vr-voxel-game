#ifndef CAMERA_H
#define CAMERA_H

#include <glad/gl.h>

/**
 * @brief A struct containing data about a camera.
 */
typedef struct {
    /// The camera position in the world.
    vec3 eye;
    /// The orientation versor/quaternion.
    versor ori;
    /// A matrix containing right-up-front vectors in that order
    /// Must be kept up to date when updating ori
    mat3 ruf;
} camera_t;

/**
 * @brief Initialises a camera object with sensible defaults.
 * @param c Pointer to a camera
 */
void camera_init(camera_t *c);

/**
 * @brief Creates a view matrix from a camera object.
 * @param c Pointer to a camera
 * @param dest Destination matrix
 */
void camera_createView(camera_t *c, mat4 dest);

/**
 * @brief Sets the position of the camera.
 * @param c Pointer to a camera
 * @param p New position
 */
void camera_setPos(camera_t *c, vec3 p);

/**
 * @brief Translates a camera relative to its orientation.
 * @param c Pointer to a camera
 * @param v Vector to translate by
 */
void camera_translate(camera_t *c, vec3 v);

/**
 * @brief Translates a camera's X component relative to its orientation.
 * @param c Pointer to a camera
 * @param dX Scalar to translate by
 */
void camera_translateX(camera_t *c, float dX);

/**
 * @brief Translates a camera's Y component relative to its orientation.
 * @param c Pointer to a camera
 * @param dY Scalar to translate by
 */
void camera_translateY(camera_t *c, float dY);

/**
 * @brief Translates a camera's Z component relative to its orientation.
 * @param c Pointer to a camera
 * @param dZ Scalar to translate by
 */
void camera_translateZ(camera_t *c, float dZ);

/**
 * @brief Changes a camera's orientation based on mouse delta.
 * @param c Pointer to a camera
 * @param dX Change in mouse X position
 * @param dY Change in mouse Y position
 */
void camera_fromMouse(camera_t *c, float dX, float dY);

/**
 * @brief Sets the view matrix of a shader program.
 * @note The view matrix should be a uniform called "View"
 * @param c Pointer to a camera
 * @param program Shader program
 */
void camera_setView(camera_t *c, GLuint program);

/**
 * Macro to get front vector of camera.
 * @param c A pointer to a camera
 */
#define camera_front(c) c->ruf[2]

/**
 * Macro to get up vector of camera.
 * @param c A pointer to a camera
 */
#define camera_up(c) c->ruf[1]

/**
 * Macro to get right vector of camera.
 * @param c A pointer to a camera
 */
#define camera_right(c) c->ruf[0]

/**
 * @brief Updates the camera view from the IMU
 * @param c A pointer to a camera
 */
void camera_update(camera_t *c); // update the camera view from imu

#endif