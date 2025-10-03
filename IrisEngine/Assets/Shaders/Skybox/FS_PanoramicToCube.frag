#version 450

layout(location = 0) in vec3 LocalPos;

layout(location = 0) out vec4 FragColor;

layout(set = 1, binding = 0) uniform sampler2D PanoramicTexture;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(LocalPos));
    vec3 color = texture(PanoramicTexture, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}