#ifndef MATERIAL_DESCRIPTOR
    #define MATERIAL_DESCRIPTOR 2
#endif

#define IE_MATERIAL_ALBEDO_MAP      (0x1 << 0x0)
#define IE_MATERIAL_NORMAL_MAP      (0x1 << 0x1)
#define IE_MATERIAL_ROUGHNESS_MAP   (0x1 << 0x2)
#define IE_MATERIAL_METALLIC_MAP    (0x1 << 0x3)
#define IE_MATERIAL_TRANSPARENT     (0x1 << 0x4)
#define IE_MATERIAL_GLTF            (0x1 << 0x5)

layout(push_constant) uniform MaterialUBO
{   
    int Parameters;
    float Roughness;
    float Metallic;
    
    float Padding;

    vec4 Albedo;
}MaterialData;

layout(set = MATERIAL_DESCRIPTOR, binding = 0) uniform sampler2D albedoMap;
layout(set = MATERIAL_DESCRIPTOR, binding = 1) uniform sampler2D roughnessMap;
layout(set = MATERIAL_DESCRIPTOR, binding = 2) uniform sampler2D metallicMap;
layout(set = MATERIAL_DESCRIPTOR, binding = 3) uniform sampler2D normalMap;

vec4 SampleAlbedo(vec2 _textCords)
{
    if(bool(MaterialData.Parameters & IE_MATERIAL_ALBEDO_MAP))
    {
        if(bool(MaterialData.Parameters & IE_MATERIAL_TRANSPARENT))
        {
            return texture(albedoMap, _textCords).rgba * MaterialData.Albedo.rgba;
        }

        return vec4(texture(albedoMap, _textCords).rgb, 1.0) * vec4(MaterialData.Albedo.rgb, 1.0);
    }

    return MaterialData.Albedo;
}

float SampleRoughness(vec2 _textCords)
{
    if(bool(MaterialData.Parameters & IE_MATERIAL_GLTF) && 
        bool(MaterialData.Parameters & IE_MATERIAL_ROUGHNESS_MAP))
    {
        return texture(roughnessMap, _textCords).g;
    }

    if(bool(MaterialData.Parameters & IE_MATERIAL_ROUGHNESS_MAP))
    {
        return texture(roughnessMap, _textCords).r;
    }

     return MaterialData.Roughness;
}

float SampleMetallic(vec2 _textCords)
{
    if(bool(MaterialData.Parameters & IE_MATERIAL_GLTF) && 
        bool(MaterialData.Parameters & IE_MATERIAL_ROUGHNESS_MAP))
    {
        return texture(roughnessMap, _textCords).b;
    }

    if(bool(MaterialData.Parameters & IE_MATERIAL_METALLIC_MAP))
    {
        return texture(metallicMap, _textCords).r;
    }

     return MaterialData.Metallic;
}

vec3 SampleNormal(vec3 _normal, mat3 _TBN, vec2 _textCords)
{
    if(bool(MaterialData.Parameters & IE_MATERIAL_NORMAL_MAP))
    {
        vec4 normal = texture(normalMap, _textCords).rgba * 2.0 - 1.0;
        return normalize(_TBN * vec3(normal));
    }

     return _normal;
}