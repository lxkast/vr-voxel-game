#pragma once

#include <stdint.h>

typedef struct {
    uint64_t state;
} rng_t;

void rng_init(rng_t *rng, uint64_t seed);

uint64_t rng_ull(rng_t *rng);

float rng_float(rng_t *rng);


float noise_value(int x, int z);

float noise_smoothValue(float x, float y);

float noise_height(int x, int z);
