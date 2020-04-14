#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 3) in vec3 colorMul;
layout (location = 4) in vec3 colorAdd;
layout (location = 5) in mat4 model;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 ColorMul;
out vec3 ColorAdd;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    TexCoords = aTexCoords;
    ColorMul = colorMul;
    ColorAdd = colorAdd;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
