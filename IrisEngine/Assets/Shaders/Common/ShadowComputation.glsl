#ifndef SHADOW_DESCRIPTOR
    #define SHADOW_DESCRIPTOR 4
#endif

#define IE_MAX_DIRECTIONAL_LIGHTS_SHADOWS   1
#define IE_MAX_SPOT_LIGHTS_SHADOWS          8
#define IE_MAX_POINT_LIGHTS_SHADOWS         4

layout(set = SHADOW_DESCRIPTOR, binding = 0) uniform sampler2D directionalShadowMaps[IE_MAX_DIRECTIONAL_LIGHTS_SHADOWS];
layout(set = SHADOW_DESCRIPTOR, binding = 1) uniform sampler2D spotShadowMaps[IE_MAX_SPOT_LIGHTS_SHADOWS];
layout(set = SHADOW_DESCRIPTOR, binding = 2) uniform samplerCube pointShadowMaps[IE_MAX_POINT_LIGHTS_SHADOWS];

///////////////////////////////////////////////////////////////////////

/// Shadow functions

///////////////////////////////////////////////////////////////////////

float CalulateDirectionalShadow(vec4 _fragPosLightSpace, vec3 _normal, vec4 _lightDir, int _index)
{
    vec3 projCoords = _fragPosLightSpace.xyz / _fragPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    float shadow = 0.0;
    float minBias = 0.001;
    float maxBias = 0.01;
    float bias = max(minBias, maxBias * (1.0 - dot(_normal, normalize(_lightDir.xyz)))); 

    vec2 texelSize = 1.0 / textureSize(directionalShadowMaps[_index], 0);
    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(directionalShadowMaps[_index], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float CalulateSpotLightShadow(vec4 _fragPosLightSpace, vec3 _normal, vec4 _lightDir, int _index)
{
    vec3 projCoords = _fragPosLightSpace.xyz / _fragPosLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    float shadow = 0.0;
    float minBias = 0.000001;
    float maxBias = 0.01;
    float bias = max(minBias, maxBias * (1.0 - dot(_normal, normalize(_lightDir.xyz)))); 

    vec2 texelSize = 1.0 / textureSize(spotShadowMaps[_index], 0);
    
    for(int x = -2; x <= 2; ++x)
    {
        for(int y = -2; y <= 2; ++y)
        {
            float pcfDepth = texture(spotShadowMaps[_index], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 25.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}


float CalculatePointShadow(vec4 _lightPosition, vec4 _fragPos, vec3 _viewPos, vec3 _normal, float _far, int _index)
{
    vec3 fragToLight = vec3(_fragPos) - vec3(_lightPosition);

    float closestDepth = texture(pointShadowMaps[_index], fragToLight).r;

    closestDepth *= _far;

    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    float samples = 20;

    float viewDistance = length(_viewPos - vec3(_fragPos));

    float diskRadius = (1.0 + (viewDistance / _far)) / 25.0;

    vec3 sampleOffsetDirections[20] = vec3[]
    (
       vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
       vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
       vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
       vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
       vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    ); 

   for(int i = 0; i < samples; ++i)
   {
        float closestDepth = texture(pointShadowMaps[_index], fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= _far;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
   }

    return shadow / samples;
}