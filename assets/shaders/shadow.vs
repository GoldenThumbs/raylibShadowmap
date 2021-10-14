#version 330

// Input vertex attributes
in vec3 vertexPosition;

// Input uniform values
uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}