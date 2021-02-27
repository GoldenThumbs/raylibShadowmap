#version 330

// Input vertex attributes
in vec3 vertexPosition;

// Input uniform values
uniform mat4 matModel;
uniform mat4 matLight;

void main()
{
    gl_Position = matLight * matModel * vec4(vertexPosition, 1.0);
}