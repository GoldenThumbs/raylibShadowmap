#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec4 shadowPos;

// Input uniform values
uniform sampler2D shadowMap; // Shadowmap
uniform vec4 colDiffuse;
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform float lightCutoff;
uniform vec3 viewPos;

// Output fragment color
out vec4 finalColor;

// Size of Shadowmap texel (probably shouldn't be hardcoded)
const vec2 texelSize = 1.0 / vec2(1024);

// Also shouldn't be hardcoded
const float lightRadius = 4.0;
const float ambient = 0.05;
const float spotSoftness = 0.65;

float ShadowCalc(vec4 p, float bias)
{
    vec3 projCoords = p.xyz / p.w;
    projCoords = projCoords * 0.5 + 0.5;
    projCoords.z = projCoords.z - bias;
	float depth = projCoords.z;

	float texDepth = texture(shadowMap, projCoords.xy).r;
	float shadow = step(depth, texDepth);

	if(projCoords.z > 1.0 || projCoords.x > 1.0 || projCoords.y > 1.0)
        shadow = 1.0;

    return shadow;
}

void main()
{
	vec3 lightRaw = lightPos - fragPosition;
	vec3 lightVec = normalize(lightRaw);
	float lightDistSqr = dot(lightRaw, lightRaw);
	vec3 viewVec = normalize(viewPos - fragPosition);
	vec3 h = normalize(lightVec + viewVec);

	float attenuation = clamp(1.0 - lightDistSqr/(lightRadius*lightRadius), 0, 1);
    attenuation *= attenuation;

	float NdL = max(0.0, dot(lightVec, fragNormal));
	float NdH = max(0.0, dot(h, fragNormal));
	float theta = dot(-lightVec, normalize(lightDir));
	float epsilon = lightCutoff - spotSoftness;
	float spot = clamp((theta - lightCutoff) / epsilon, 0.0, 1.0);

	float diff = NdL * attenuation;

	float bias = max(0.001 * (1.0 - NdL), 0.0001);
   	diff *= ShadowCalc(shadowPos, bias) * spot;

	float spec = pow(NdH, 64.0) * diff;

	finalColor.rgb = (colDiffuse.rgb * diff + spec) + colDiffuse.rgb * ambient;
    finalColor.a = 1.0;
}