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
    int type = types[int(vPos.z) + 16 * int(vPos.y) + 16 * 16 * int(vPos.x)];
    if (type == 0) {
        discard;
    } else {
        FragColor = vec4(1,1,1,1);
    }
}


//    int type = types[int(vPos.z) + 16 * int(vPos.y) + 16 * 16 * int(vPos.x)];
//    if (type == 0) {
//        discard;
//    }
//    vec2 texCoord = vPos.xy;
//    if (texCoord.y < 0.001 || texCoord.y > 0.999) {
//        texCoord.y = vPos.z;
//    } else if (texCoord.x < 0.001 || texCoord.x > 0.999) {
//        texCoord.x = vPos.x;
//    }