#version 140
#define TEXTURE_WIDTH 16
#define TEXTURE_HEIGHT 16
#define ATLAS_WIDTH 128
#define ATLAS_HEIGHT 64

flat in int vTexIndex;
in float vFogDepth;
in vec3 vPos;

out vec4 FragColor;

uniform sampler2D uTextureAtlas;
uniform float fogStart;
uniform float fogEnd;

void main() {
    vec4 fogColor = vec4(135.f/255.f, 206.f/255.f, 235.f/255.f, 1.0f);

    float f = 0;
    float denom = fogEnd - fogStart;
    if (denom <= 0.f) {
        f = 1.f;
    } else {
        f = (fogEnd - vFogDepth) / denom;
    }
    f = clamp(f, 0.f, 1.f);

    vec3 frac = fract(vPos);
    vec2 fracCoord;
    if (frac.x < 0.001 || frac.x > 0.999) {
        fracCoord = frac.zy;
    } else if (frac.y < 0.001 || frac.y > 0.999) {
        fracCoord = frac.xz;
    } else {
        fracCoord = frac.xy;
    }
    int textureCol = vTexIndex / 4;
    int textureRow = vTexIndex % 4;
    vec2 texCoords = vec2(((fracCoord.x + textureCol) * TEXTURE_WIDTH) / ATLAS_WIDTH,
                            ((fracCoord.y + textureRow) * TEXTURE_HEIGHT) / ATLAS_HEIGHT);
    vec4 finalRGB = mix(fogColor, texture(uTextureAtlas, texCoords), f);
    if (finalRGB.a == 0.0) {
        discard;
    }
    FragColor = finalRGB;
}