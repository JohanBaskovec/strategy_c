#version 330 core
in vec3 aPos;
in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
