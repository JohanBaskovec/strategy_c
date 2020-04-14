#version 330 core

uniform sampler2D diffuse;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 ColorAdd;
in vec3 ColorMul;

out vec4 FragColor;


void main()
{
    vec3 result = texture(diffuse, TexCoords).rgb;
    result += ColorAdd;
    result *= ColorMul;

    float gamma = 2.2;
    vec3 color = pow(result, vec3(1.0/gamma));
    FragColor = vec4(color, 1.0);
}
