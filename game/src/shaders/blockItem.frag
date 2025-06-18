#version 140

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTextureAtlas;
uniform vec2 texOffset;

void main() {
    vec4 finalRGB = texture(uTextureAtlas, vTexCoord + texOffset);
    FragColor = finalRGB;
}