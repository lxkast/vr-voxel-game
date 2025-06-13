#pragma once

#include <stdint.h>

typedef struct {
    uint64_t state;
} rng_t;

void rng_init(rng_t *rng, uint64_t seed);

uint64_t rng_ull(rng_t *rng);

float rng_float(rng_t *rng);

/**
 * @brief Generates a random number in the range (min, max)
 * @param rng A pointer to an rng
 * @param min The minimum value the random number can take
 * @param max The maximum value the random number can take
 * @return the random number
 */
float rng_floatRange(rng_t *rng, const float min, const float max);

float noise_value(int x, int z);

float noise_smoothValue(float x, float y);

float noise_height(int x, int z);
