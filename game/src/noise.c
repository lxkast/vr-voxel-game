#include <cglm/cglm.h>
#include "math.h"
#include "noise.h"

/**
 * @brief A mixer function to make a pseudorandom 64-bit number
 * @note https://gee.cs.oswego.edu/dl/papers/oopsla14.pdf
 * @param x The number to mix
 * @return The new number
 */
static uint64_t mix64(const uint64_t x) {
    // Standard implementation of the splitmix64 function, see note in docstring
    uint64_t z = x;
    z = (z ^ z >> 30) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ z >> 27) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

static void rng_update(rng_t *rng) {
    rng->state += 0x9E3779B97F4A7C15ULL;
}

void rng_init(rng_t *rng, const uint64_t seed) {
    rng->state = seed;
}

uint64_t rng_ull(rng_t *rng) {
    rng_update(rng);
    return mix64(rng->state);
}

float rng_float(rng_t *rng) {
    return (float)rng_ull(rng) / (float)UINT64_MAX;
}

float rng_floatRange(rng_t *rng, const float min, const float max) {
    return min + (max - min) * rng_float(rng);
}

float noise_value(const noise_t *n, const int x, const int y) {
    const uint64_t z0 = mix64(57 * x ^ 97 * y);
    const int z1 = 0x7FFFFFFF & (int)mix64(z0 ^ n->seed);

    return 1.0f - ((float)z1 / 1073741824.f);
}

static float ease(const float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float noise_smoothValue(const noise_t *n, const float x, const float y) {
    const int xInt = (int)floorf(x);
    const int yInt = (int)floorf(y);
    const float xFrac = ease(x - (float)xInt);
    const float yFrac = ease(y - (float)yInt);


    const float v00 = noise_value(n, xInt, yInt);
    const float v10 = noise_value(n, xInt + 1, yInt);
    const float v01 = noise_value(n, xInt, yInt + 1);
    const float v11 = noise_value(n, xInt + 1, yInt + 1);

    const float i1 = glm_lerp(v00, v10, xFrac);
    const float i2 = glm_lerp(v01, v11, xFrac);
    return glm_lerp(i1, i2, yFrac);
}

float noise_fbm(noise_t *n,
                const float x,
                const float y,
                const int octaves,
                const float persistence,
                const float baseFrequency) {
    float total = 0.f;
    float frequency = baseFrequency;
    float amplitude = 1.f;
    float maxAmplitude = 0.f;

    for (int i = 0; i < octaves; i++) {
        total += noise_smoothValue(n, x * frequency, y * frequency) * amplitude;
        maxAmplitude += amplitude;
        amplitude *= persistence;
        frequency *= 2.f;
    }

    return total / maxAmplitude;
}
