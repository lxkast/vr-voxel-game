#version 140
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D leftTexture;
uniform sampler2D rightTexture;
float distortionStrength = 0.2f;

vec2 distortEye(vec2 inTexCoords) {
    vec2 normalizedCoords = inTexCoords * 2.0 - 1.0;

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
    return finalTexCoords;

    // if texture coords outside of [0,1], render pixel as black
//    if (finalTexCoords.x > 1.0 || finalTexCoords.x < 0.0 ||
//    finalTexCoords.y > 1.0 || finalTexCoords.y < 0.0)
//    {
//        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
//    }
//    else
//    {
//        FragColor = texture(screenTexture, finalTexCoords);
//    }
}

void main()
{
    if (TexCoords.x >= 0.5) {
        vec2 outTexCoords = distortEye(vec2(2 * (TexCoords.x - 0.5), TexCoords.y));
        if (outTexCoords.x > 1.0 || outTexCoords.x < 0.0 ||
        outTexCoords.y > 1.0 || outTexCoords.y < 0.0)
            {
                FragColor = vec4(0.0, 0.0, 0.0, 1.0);
            }
            else
            {
                FragColor = texture(rightTexture, outTexCoords);
            }
    } else {
        vec2 outTexCoords = distortEye(vec2(2 * (TexCoords.x), TexCoords.y));
        if (outTexCoords.x > 1.0 || outTexCoords.x < 0.0 ||
        outTexCoords.y > 1.0 || outTexCoords.y < 0.0)
        {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
        else
        {
            FragColor = texture(leftTexture, outTexCoords);
        }
    }
}