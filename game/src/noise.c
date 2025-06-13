#include "noise.h"

#include "math.h"
#include <cglm/cglm.h>

/**
 * @brief A mixer function
 * @note https://gee.cs.oswego.edu/dl/papers/oopsla14.pdf
 * @param rng The random number generator
 * @return The new number
 */
static uint64_t mix64(rng_t *rng) {
    uint64_t z = rng->state += 0x9E3779B97F4A7C15ULL;
    z = (z ^ z >> 30) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ z >> 27) * 0x94D049BB133111EBULL;
    return z ^ z >> 31;
}

void rng_init(rng_t *rng, const uint64_t seed) {
    rng->state = seed;
}

uint64_t rng_ull(rng_t *rng) {
    return mix64(rng);
}

float rng_float(rng_t *rng) {
    return (float)mix64(rng) / (float)0xFFFFFFFFFFFFFFFFULL;
}

float rng_floatRange(rng_t *rng, const float min, const float max) {
    return min + (max - min) * rng_float(rng);
}

float noise_value(const int x, const int y) {
    int n = x + y * 57;
    n = (n << 13) ^ n;
    const int nn = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return 1.0f - ((float)nn / 1073741824.0f);
}

static float ease(const float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float noise_smoothValue(const float x, const float y) {
    const int x_int = (int)floorf(x);
    const int y_int = (int)floorf(y);
    const float x_frac = ease(x - (float)x_int);
    float y_frac = ease(y - (float)y_int);


    const float v00 = noise_value(x_int,     y_int);
    const float v10 = noise_value(x_int + 1, y_int);
    const float v01 = noise_value(x_int,     y_int + 1);
    const float v11 = noise_value(x_int + 1, y_int + 1);

    const float i1 = glm_lerp(v00, v10, x_frac);
    const float i2 = glm_lerp(v01, v11, x_frac);
    return glm_lerp(i1, i2, y_frac);
}

float noise_height(const int x, const int z) {
    float totalHeight = 0.f;
    float frequency = 0.01f;
    float amplitude = 1.f;
    float maxAmplitude = 0.f;

    for (int octave = 0; octave < 4; octave++) {
        totalHeight += noise_smoothValue((float)x * frequency, (float)z * frequency) * amplitude;
        maxAmplitude += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.f;
    }

    return totalHeight / maxAmplitude;
}
