#version 330

#define M_PI 3.14159265358979323846
#define M_TWOPI 6.283185307179586

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

const int taps = 9;

const vec2 kernel[9] = vec2[](
    vec2(0, 0),
	vec2( 1, 0), vec2( 0, 1),
    vec2(-1, 0), vec2( 0,-1),
    vec2(.8,-.8), vec2(.8, .8),
    vec2(-.8,.8), vec2(-.8,-.8)
);
const float weight[9] = float[](
    1.0,
	0.5, 0.5,
    0.5, 0.5,
    0.25, 0.25,
    0.25, 0.25
);

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float ShadowCalc(vec4 p, float bias)
{
    vec3 projCoords = p.xyz / p.w;
    projCoords = projCoords * 0.5 + 0.5;
    float depth = projCoords.z - bias;

    // offset pattern math taken from: 
    // https://developer.nvidia.com/gpugems/gpugems/part-ii-lighting-and-shadows/chapter-11-shadow-map-antialiasing
    vec2 offset = step(fract(gl_FragCoord.xy * 0.5), vec2(0.25));
    offset.y += offset.x;
    if (offset.y > 1.1)
        offset.y = 0;

    float shadow = 0;
    float wTotal = 0;

    for (int i = 0; i < taps; i++)
    {
        float w = weight[i];
        vec3 shadowCoord;
        shadowCoord.xy = projCoords.xy + (kernel[i] + offset) * texelSize;
        shadowCoord.z = depth;
        // Because we are using a shadow sampler, the texture function takes a vec3
        // xy = shadow coords (special texture coordinates that transform the shadowmap)
        // z = depth transformed to light-space (we compare the shadowmap depth values to this)
        shadow += texture(texture0, shadowCoord)*w;
        wTotal += w;
    }
    shadow /= wTotal;
    if(projCoords.z > 1.0)
        shadow = 1.0;
    return shadow;
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

    float bias = max(0.001 * (1.0 - nDl), 0.0001); // Use slope bias unless constant bias is larger
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