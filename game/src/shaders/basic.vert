#version 140

in vec3 aPos;
in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 vTexCoord;
out float vFogDepth;

void main() {
    vec4 eyePos = view * model * vec4(aPos, 1.0f);

    vFogDepth = -eyePos.z;

    gl_Position = projection * eyePos;
    vTexCoord = aTexCoord;
}