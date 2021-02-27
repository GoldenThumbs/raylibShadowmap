#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matModel;
//uniform mat4 matView;
uniform mat4 matLight;

uniform vec3 lightPos;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec3 fragNormal;
out vec4 fragPosLS;

// NOTE: Add here your custom variables
const float normalOffset = 0.1;

void main()
{
    fragPosition = vec3(matModel*vec4(vertexPosition, 1.0));
    
    mat3 normalMatrix = transpose(inverse(mat3(matModel)));
    fragNormal = normalize(normalMatrix*vertexNormal);

    vec3 lightVec = normalize(lightPos - fragPosition);
    float cosAngle = clamp(1.0 - dot(lightVec, fragNormal), 0, 1);
    vec3 scaledNormalOffset = fragNormal * (normalOffset * cosAngle);

    fragPosLS = matLight * vec4(fragPosition + scaledNormalOffset, 1.0);

    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}