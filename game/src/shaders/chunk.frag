#version 140

in vec3 vPos;
out vec4 FragColor;
uniform sampler2D uTextureAtlas;
uniform isampler3D uTypes;

float textureWidth = 16.0;
float textureHeight = 16.0;
float atlasWidth = 96.0;
float atlasHeight = 32.0;

// intPos refers to the position in the original blocks array
// GLSL's 3D textures are in column major, so indexing into the
// texture at z,y,x yields x,y,z in the original block array
void main() {
    ivec3 intPos = ivec3(int(vPos.z), int(vPos.y), int(vPos.x));
    vec3 pos = vPos;
    if (intPos.z >= 16) {
        pos.x = 15.9999;
        intPos.z = 15;
    }
    if (intPos.y >= 16) {
        pos.y = 15.9999;
        intPos.y = 15;
    }
    if (intPos.x >= 16) {
        pos.z = 15.9999;
        intPos.x = 15;
    }
    int type = texelFetch(uTypes, intPos, 0).r;

    // if the current block is air but is neighbouring a solid block, make the face solid
    if (type == 0) {
        if (mod(pos.x, 1.0) < 0.001) {
            if (intPos.z > 0) {
                ivec3 neighbor = intPos;
                neighbor.z -= 1;
                type = texelFetch(uTypes, neighbor, 0).r;
                if (type == 0) {
                    discard;
                }
            } else {
                discard;
            }
        } else if (mod(pos.x, 1.0) > 0.999) {
            if (intPos.z < 15) {
                ivec3 neighbor = intPos;
                neighbor.z += 1;
                type = texelFetch(uTypes, neighbor, 0).r;
                if (type == 0) {
                    discard;
                }
            } else {
                discard;
            }
        } else if (mod(pos.y, 1.0) < 0.001) {
            if (intPos.y > 0) {
                ivec3 neighbor = intPos;
                neighbor.y -= 1;
                type = texelFetch(uTypes, neighbor, 0).r;
                if (type == 0) {
                    discard;
                }
            } else {
                discard;
            }
        } else if (mod(pos.y, 1.0) > 0.999) {
            if (intPos.y < 15) {
                ivec3 neighbor = intPos;
                neighbor.y += 1;
                type = texelFetch(uTypes, neighbor, 0).r;
                if (type == 0) {
                    discard;
                }
            } else {
                discard;
            }
        } else if (mod(pos.z, 1.0) < 0.001) {
            if (intPos.x > 0) {
                ivec3 neighbor = intPos;
                neighbor.x -= 1;
                type = texelFetch(uTypes, neighbor, 0).r;
                if (type == 0) {
                    discard;
                }
            } else {
                discard;
            }
        } else if (mod(pos.z, 1.0) > 0.999) {
            if (intPos.x < 15) {
                ivec3 neighbor = intPos;
                neighbor.x += 1;
                type = texelFetch(uTypes, neighbor, 0).r;
                if (type == 0) {
                    discard;
                }
            } else {
                discard;
            }
        } else {
            discard;
        }
    }

    vec2 texCoord = pos.xy;
    bool below = false;
    if (mod(texCoord.y, 1.0) < 0.001 || mod(texCoord.y, 1.0) > 0.999) {
        texCoord.y = pos.z;
        below = true;
    } else if (mod(texCoord.x, 1.0) < 0.001 || mod(texCoord.x, 1.0) > 0.999) {
        texCoord.x = pos.z;
    }
    FragColor = texture(uTextureAtlas, vec2((mod(texCoord.x, 1.0) * 16 + 16.0 * float(type))/atlasWidth, (16 * mod(texCoord.y,1.0) + (below ? 16.0 : 0.0))/atlasHeight));
}