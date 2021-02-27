#version 330

in vec2 fragTexCoord;

// Output fragment color
out vec4 finalColor;

uniform sampler2D texture0;

const float nearClip = 0.1;
const float farClip = 50.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * nearClip * farClip) / (farClip + nearClip - z * (farClip - nearClip));
}

void main()
{
    float depth = LinearizeDepth(texture(texture0, fragTexCoord).r);
    finalColor.rgb = vec3(depth) / farClip;
    finalColor.a = 1.0;
}