#version 140

in vec3 aPos;
in int aTexIndex;
in float aLightValue;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

flat out int vTexIndex;
out float vLightValue;
out float vFogDepth;
out vec3 vPos;

void main() {
    vec4 eyePos = view * model * vec4(aPos, 1.0f);

    vFogDepth = -eyePos.z;

    gl_Position = projection * eyePos;
    vTexIndex = aTexIndex;
    vPos = aPos;
    vLightValue = aLightValue;
}