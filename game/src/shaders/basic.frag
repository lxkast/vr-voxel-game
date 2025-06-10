#version 140

in vec2 vTexCoord;
in float vFogDepth;

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


    vec4 finalRGB = mix(fogColor, texture(uTextureAtlas, vTexCoord), f);
    FragColor = finalRGB;
}