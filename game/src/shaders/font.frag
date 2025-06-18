#version 140

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTextureAtlas;
uniform vec2 texOffset;
uniform vec4 textColour;

void main() {
    vec4 finalR = texture(uTextureAtlas, vTexCoord + texOffset);
    if (finalR == vec4(0, 0, 0, 1)) {
        discard;
    }
    FragColor = finalR * textColour;
}