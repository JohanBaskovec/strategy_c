#version 330 core
in vec3 aPos;
in vec2 aTexCoords;
in vec3 colorMul;
in vec3 colorAdd;
in mat4 model;
in int keep;

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
    if (keep == 0) {
        gl_Position.x += 100;
    }
}
