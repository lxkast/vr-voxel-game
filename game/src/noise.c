#include "noise.h"

#include "math.h"
#include <cglm/cglm.h>

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
