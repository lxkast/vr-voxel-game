#version 140

in vec3 vPos;
out vec4 FragColor;
uniform sampler2D uTextureAtlas;
uniform isampler3D uTypes;

float textureWidth = 16.0;
float textureHeight = 16.0;
float atlasWidth = 96.0;
float atlasHeight = 32.0;

void main() {
    ivec3 intPos = ivec3(int(vPos.x), int(vPos.y), int(vPos.z));
    int type = texelFetch(uTypes, intPos, 0).r;
    if (type == 0) {
        discard;
    }
    vec2 texCoord = vPos.xy;
    bool below = false;
    if (mod(texCoord.y, 1.0) < 0.001 || mod(texCoord.y, 1.0) > 0.999) {
        texCoord.y = vPos.z;
        below = true;
    } else if (mod(texCoord.x, 1.0) < 0.001 || mod(texCoord.x, 1.0) > 0.999) {
        texCoord.x = vPos.z;
    }
    FragColor = texture(uTextureAtlas, vec2((mod(texCoord.x, 1.0) * 16 + 16.0 * float(type))/atlasWidth, (16 * mod(texCoord.y,1.0) + (below ? 16.0 : 0.0))/atlasHeight));
    //FragColor = vec4((float(type) * float(textureWidth) + texCoord.x), 1,1,1);
}