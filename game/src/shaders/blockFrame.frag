#version 140

in vec2 vTexCoord;

out vec4 FragColor;

uniform vec4 colour;
uniform vec2 texOffset;

void main() {
    vec4 finalRGB = colour;
    FragColor = finalRGB;
}