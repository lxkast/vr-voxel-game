#version 140

out vec4 FragColor;
in vec2 vTexCoord;
uniform sampler2D uTextureAtlas;

void main() {
    FragColor = texture(uTextureAtlas, vTexCoord);
}