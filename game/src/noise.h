#pragma once

#include <stdint.h>

/**
 * @brief A struct representing a random number generator
 * @note Uses the splitmix64 algorithm
 * @note https://gee.cs.oswego.edu/dl/papers/oopsla14.pdf
 */
typedef struct {
    /// The 64 bit state of the random number generator
    uint64_t state;
} rng_t;

/**
 * @brief Initialises an rng with a seed
 *
 * @param rng A pointer to an rng
 * @param seed The seed
 */
void rng_init(rng_t *rng, uint64_t seed);

/**
 * @brief Generates a random 64 bit number
 * 
 * @param rng A pointer to an rng
 * @return uint64_t A random 64 bit number
 */
uint64_t rng_ull(rng_t *rng);

/**
 * @brief Generates a random (32 bit) float
 * 
 * @param rng A pointer to an rng
 * @return float A random float
 */
float rng_float(rng_t *rng);

/**
 * @brief Generates a random float in the range (min, max)
 * 
 * @param rng A pointer to an rng
 * @param min The minimum value the random number can take
 * @param max The maximum value the random number can take
 * @return float The random number
 */
float rng_floatRange(rng_t *rng, const float min, const float max);


/**
 * @brief A struct representing a noise maker
 */
typedef struct {
    /// The seed to use for noise generation
    uint32_t seed;
} noise_t;

/**
 * @brief A function to generate value noise
 * 
 * @param n A pointer to a noise object
 * @param x The first input
 * @param z The second input
 * @return float The random noise value
 * @note Output range is [-1, 1]
 */
float noise_value(noise_t *n, int x, int z);

/**
 * @brief A function to generate smooth value noise
 * 
 * @param n A pointer to a noise object
 * @param x The first input
 * @param y The second input
 * @return float The random noise value
 * @note Output range is [-1, 1]
 */
float noise_smoothValue(noise_t *n, float x, float y);

/**
 * @brief A fractal brownian motion function
 *
 * @param n A pointer to a noise object
 * @param x The first input
 * @param y The second input
 * @param octaves The number of octaves of noise
 * @param persistence The persistence
 * @param baseFrequency The base frequency
 * @return float The value from the noise
 */
float noise_fbm(noise_t *n, float x, float y, int octaves, float persistence, float baseFrequency);