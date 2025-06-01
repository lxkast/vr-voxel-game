#version 140

in vec3 vPos;
out vec4 FragColor;
uniform sampler2D uTextureAtlas;

layout (std140) uniform ChunkData
{
    int types[4096];
};

int textureWidth = 16;
int textureHeight = 16;
int atlasWidth = 96;
int atlasHeight = 16;

void main() {
    FragColor = vec4(sin(vPos.x), sin(vPos.y), sin(vPos.z), 1);
}