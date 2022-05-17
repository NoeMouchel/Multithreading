#version 450 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
    TexCoords = aPos;
    gl_Position = Projection * mat4(mat3(View)) * vec4(aPos, 1.0);
}  