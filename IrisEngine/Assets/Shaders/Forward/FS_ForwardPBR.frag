#version 450
#extension GL_GOOGLE_include_directive : enable

#include "MaterialData.glsl"
#include "LightData.glsl"
#include "PBRComputation.glsl"
#include "ShadowComputation.glsl"
#include "ImageBasedLighting.glsl"

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 textCoord;
layout(location = 2) in vec3 vertexWorldPosition;
layout(location = 3) in vec4 cameraPosition;
layout(location = 4) in mat3 TBN;

layout(location = 0) out vec4 fragColor;

void main() 
{
	vec3 normalVec = SampleNormal(normal, TBN, textCoord);
    vec4 albedoValue = SampleAlbedo(textCoord);
    float roughness = SampleRoughness(textCoord);
    float metallic = SampleMetallic(textCoord);

    vec3 V = normalize(cameraPosition.xyz - vertexWorldPosition);
	
	vec4 globalLight = vec4 (0.0);
	vec4 finalLight = vec4(0.0);
	vec3 brdf = vec3(0.0);
    
    vec3 specular = vec3(0.04);

    vec4 LightOutput = vec4(0.0, 0.0, 0.0, 1.0);

    int shadowIndex = 0;

    // Directional Light

    for(int i = 0; i < LightDataUBO.dirLightCount; ++i)
    {
        finalLight = CalculateDirectionalLight(LightDataUBO.dirLight[i], normalVec);

        float shadow = 1.0;

        if(LightDataUBO.dirLight[i].calculateShadow)
        {
            shadow = CalulateDirectionalShadow(LightDataUBO.dirLight[i].lightSpaceMatrix * vec4(vertexWorldPosition, 1.0), normalVec, -LightDataUBO.dirLight[i].direction, shadowIndex);
            ++shadowIndex;
        }

        brdf = BRDF(V, normalVec, -normalize(vec3(LightDataUBO.dirLight[i].direction)), roughness, metallic, vec3(albedoValue), specular, (1.0 - shadow));

        LightOutput += vec4(brdf, 1.0) * vec4(finalLight.rgb, 1.0);
    }

    shadowIndex = 0;

    // Spot Light
 
    for(int i = 0; i < LightDataUBO.spotLightCount; ++i)
    {
        finalLight = CalculateSpotLight(LightDataUBO.spotLight[i], normalVec, vertexWorldPosition);

        float shadow = 1.0;

        if(LightDataUBO.spotLight[i].calculateShadow)
        {
            shadow = CalulateSpotLightShadow(LightDataUBO.spotLight[i].lightSpaceMatrix * vec4(vertexWorldPosition, 1.0), normalVec, -LightDataUBO.spotLight[i].direction, shadowIndex);
            ++shadowIndex;
        }

        brdf = BRDF(V, normalVec, normalize(vec3(LightDataUBO.spotLight[i].position) - vec3(vertexWorldPosition)), roughness, metallic, vec3(albedoValue), specular, (1.0 - shadow));
    
        LightOutput += vec4(brdf, 1.0) * vec4(finalLight.rgb, 1.0);
    }

    shadowIndex = 0;

    // Point Light

    for(int i = 0; i < LightDataUBO.pointLightCount; ++i)
    {
        finalLight = CalculatePointLight(LightDataUBO.pointLight[i], normalVec, vertexWorldPosition);

        float shadow = 1.0;

        if(LightDataUBO.pointLight[i].calculateShadow)
        {
            shadow = CalculatePointShadow(LightDataUBO.pointLight[i].position, vec4(vertexWorldPosition, 1.0), cameraPosition.xyz, normalVec, LightDataUBO.pointLight[i].lightFar, shadowIndex);
            ++shadowIndex;
        }

        brdf = BRDF(V, normalVec, normalize(vec3(LightDataUBO.pointLight[i].position) - vec3(vertexWorldPosition)), roughness, metallic, vec3(albedoValue), specular, (1.0 - shadow));
        
        LightOutput += vec4(brdf, 1.0) * vec4(finalLight.rgb, 1.0);
    }

    // IBL

    //0.16 is an aproximation of a base fressel value
    vec3 _F0 = vec3(0.16 * (specular * specular));
	_F0 = mix(_F0, vec3(albedoValue), metallic);

    vec3 F = F_SchlickRoughness(max(dot(normalVec, V), 0.0), _F0, roughness);

    vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;

	vec3 irraAmbiant = texture(IrradianceMap, normalVec).rgb;
	vec3 diffuse = irraAmbiant * vec3(albedoValue);

	vec3 reflectVector = reflect(-V, normalVec);

	const float MAX_REFLECTION_LOD = 4;

	vec3 prefiltredColor = textureLod(PrefilteredMap, reflectVector, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBRDF = texture(LookUpMap, vec2(max(dot(normalVec, V), 0.0), roughness)).rg;

	vec3 spec = prefiltredColor * (F * envBRDF.x + envBRDF.y);

	vec4 ambiantLight = vec4((kD * diffuse + spec), 1.0);

	vec4 finalColor = (ambiantLight + LightOutput);

    // FINAL

    fragColor = finalColor;
    fragColor.a = albedoValue.a;
}