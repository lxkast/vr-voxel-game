#version 140

in vec3 vPos;
out vec4 FragColor;
uniform sampler2D uTextureAtlas;
layout (std140, binding = 0) uniform ChunkData
{
    int types[4096];
};

void main() {
    FragColor = vec4(1,1,1,1);
}