#version 140
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
float distortionStrength = 0.2f;

void main()
{
    vec2 normalizedCoords = TexCoords * 2.0 - 1.0;

    float r = length(normalizedCoords);

    float r_distorted = r * (1.0 + distortionStrength * r * r);

    vec2 distortedCoords;
    if (r_distorted > 0.001) {
        distortedCoords = normalizedCoords * (r_distorted / r);
    }
    else {
        distortedCoords = normalizedCoords;
    }

    vec2 finalTexCoords = (distortedCoords + 1.0) / 2.0;

    // if texture coords outside of [0,1], render pixel as black
    if (finalTexCoords.x > 1.0 || finalTexCoords.x < 0.0 ||
    finalTexCoords.y > 1.0 || finalTexCoords.y < 0.0)
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        FragColor = texture(screenTexture, finalTexCoords);
    }
}