#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec3 fragNormal;
in vec4 fragPosLS;

// Input uniform values
uniform sampler2DShadow texture0; // Shadowmap
uniform vec4 colDiffuse;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform mat4 mvp;

// Output fragment color
out vec4 finalColor;

// Size of Shadowmap texel (probably shouldn't be hardcoded)
const vec2 texelSize = 1.0 / vec2(2048);

// Also shouldn't be hardcoded
const float lightRadius = 6.0;
const float ambient = 0.01;

float ShadowCalc(vec4 p, float bias)
{
    vec3 projCoords = p.xyz / p.w;
    projCoords = projCoords * 0.5 + 0.5;
    float depth = projCoords.z - bias;

    float shadow = 0;

    for (int x = -1; x <= 1; x++)
    for (int y = -1; y <= 1; y++)
    {
        vec3 shadowCoord;
        shadowCoord.xy = projCoords.xy + vec2(x, y) * texelSize;
        shadowCoord.z = depth;
        // Because we are using a shadow sampler, the texture function takes a vec3
        // xy = shadow coords (special texture coordinates that transform the shadowmap)
        // z = depth transformed to light-space (we compare the shadowmap depth values to this)
        shadow += texture(texture0, shadowCoord);
    }
    return shadow / 9.0;
}

void main()
{
    vec3 lRAW = lightPos - fragPosition;

    vec3 l = normalize(lRAW); // Light vec
    vec3 n = normalize(fragNormal); // Normal vec
    vec3 v = normalize(viewPos - fragPosition); // Camera/View vec
    vec3 h = normalize(l + v); // Half-way vec

    float nDl = max(dot(n, l), 0.0);
    float nDh = max(dot(n, h), 0.0);

    float bias = max(0.001 * (1.0 - nDl), 0.0005); // Use slope bias unless constant bias is larger
    float shadow = ShadowCalc(fragPosLS, bias);

    float dist = length(lRAW);
    float attenuation = clamp(1.0 - dist*dist/(lightRadius*lightRadius), 0, 1);
    attenuation *= attenuation;

    float diff = nDl * attenuation * shadow;

    float spec = pow(nDh, 64.0) * diff;

    finalColor.rgb = colDiffuse.rgb * diff + spec;

    finalColor.rgb += colDiffuse.rgb * ambient;
    finalColor.rgb = pow(finalColor.rgb, vec3(1.0/2.2));

    finalColor.a = 1.0;
}