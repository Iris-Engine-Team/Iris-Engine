#ifndef LIGHT_DESCRIPTOR
    #define LIGHT_DESCRIPTOR 3
#endif

#define IE_MAX_DIRECTIONAL_LIGHTS   2
#define IE_MAX_SPOT_LIGHTS          20
#define IE_MAX_POINT_LIGHTS         20

struct DirectionalLight     // 112 = 7 * 16
{
    vec4 direction;         // 16
    vec4 color;             // 16

    float intensity;        // 4    //
                                    //
    float lightNear;        // 4    //  16
    float lightFar;         // 4    //
                                    //
    bool calculateShadow;   // 4    //

    mat4 lightSpaceMatrix;  // 64
};


struct PointLight           // 48 = 3 * 16
{
    vec4 position;          // 16
    vec4 color;             // 16

    float intensity;        // 4    //
                                    //
    float lightNear;        // 4    //  16
    float lightFar;         // 4    //
                                    //
    bool calculateShadow;   // 4    //
};  

struct SpotLight            // 144 = 9 * 16
{
    vec4 position;          // 16
    vec4 direction;         // 16
    vec4 color;             // 16

    float intensity;        // 4    //
                                    //
    float lightNear;        // 4    //  16
    float lightFar;         // 4    //
                                    //
    float cutOff;           // 4    //
    float outerCutOff;      // 4    //  
                                    //  16
    bool calculateShadow;   // 4    //
    float padding0;         // 4    //  
    float padding1;         // 4    //  

    mat4 lightSpaceMatrix;  // 64
};

layout(set = LIGHT_DESCRIPTOR, binding = 0) uniform LightData
{
    DirectionalLight    dirLight[IE_MAX_DIRECTIONAL_LIGHTS];    // 80   * 2
    SpotLight           spotLight[IE_MAX_POINT_LIGHTS];         // 112  * 20
    PointLight          pointLight[IE_MAX_SPOT_LIGHTS];         // 48   * 20
   
    int dirLightCount;
    int spotLightCount;
    int pointLightCount;

    float padding;
}LightDataUBO;

///////////////////////////////////////////////////////////////////////

/// Light functions

///////////////////////////////////////////////////////////////////////

vec4 CalculateDirectionalLight(DirectionalLight _DirLight, vec3 _Normal)
{
	vec3 l = normalize(vec3(-_DirLight.direction));
	float dotNL = clamp(dot(_Normal, l), 0.0, 1.0);

	return _DirLight.intensity * dotNL * _DirLight.color;
}

float GetSquareFalloffAttenuation(vec3 _PosToLight, float _LightInvRadius)
{
	float distanceSquare = dot(_PosToLight, _PosToLight);
	float factor = distanceSquare * _LightInvRadius * _LightInvRadius;
	float smoothFactor = max(1.0 - factor * factor, 0.0);
	
	return (smoothFactor * smoothFactor) / max(distanceSquare, 1e-4);
}

float GetSpotAngleAttenuation(vec3 _L, vec3 _LightDir, float _CutOff, float _OuterCutOff)
{
	float cosOuter = cos(_OuterCutOff);
	float spotScale = 1.0 / max(cos(_CutOff) - cosOuter, 1e-4);
	float spotOffset = -cosOuter * spotScale;

	float cd = dot(normalize(-_LightDir), _L);
	float attenuation = clamp(cd * spotScale + spotOffset, 0.0, 1.0);

	return attenuation * attenuation;
}

vec4 CalculateSpotLight(SpotLight _SpotLight, vec3 _Normal, vec3 _WorldPosition)
{
	vec3 posToLight = vec3(_SpotLight.position) - _WorldPosition;
	vec3 l = normalize(posToLight);
	float dotNL = clamp(dot(_Normal, l), 0.0, 1.0);

	float attenuation = GetSquareFalloffAttenuation(posToLight, 1.0 / 10000.0);
	attenuation *= GetSpotAngleAttenuation(l, vec3(_SpotLight.direction), _SpotLight.cutOff, _SpotLight.outerCutOff);

	return (_SpotLight.intensity * attenuation * dotNL) * _SpotLight.color;
}

vec4 CalculatePointLight(PointLight _PointLight, vec3 _Normal, vec3 _WorldPosition)
{
	vec3 posToLight = vec3(_PointLight.position) - _WorldPosition;
	vec3 l = normalize(posToLight);
	float dotNL = clamp(dot(_Normal, l), 0.0, 1.0);

	float attenuation = GetSquareFalloffAttenuation(posToLight,  1.0 / 10000.0);

	return (_PointLight.intensity * attenuation * dotNL) * _PointLight.color;
}