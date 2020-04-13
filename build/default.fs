#version 330 core

uniform sampler2D diffuse;

in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform bool selected;

void main()
{
    vec3 result = texture(diffuse, TexCoords).rgb;
    if (selected) {
        result.r = 200;
    }

    float gamma = 2.2;
    vec3 color = pow(result, vec3(1.0/gamma));
    FragColor = vec4(color, 1.0);
}
