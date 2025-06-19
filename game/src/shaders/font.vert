#version 140

in vec3 aPos;
in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 projection;

out vec2 vTexCoord;

void main() {
    vec4 eyePos = model * vec4(aPos, 1.0f);

    gl_Position = projection * eyePos;
    vTexCoord = aTexCoord;
}