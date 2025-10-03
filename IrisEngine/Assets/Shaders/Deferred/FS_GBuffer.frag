#version 450

#include "MaterialData.glsl"

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textCoord;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outAlbedo;
layout(location = 3) out vec4 outMaterial;

void main()
{
	// Apply normal mapping and Normal buffer
    outNormal = vec4(SampleNormal(normal, TBN, textCoord), 1.0);

	// Position buffer
	outPosition = vec4(position.xyz, 1.0);

	// Albedo buffer
	outAlbedo = SampleAlbedo(textCoord);

	// Material buffer
	outMaterial = vec4(0.0, SampleRoughness(textCoord), SampleMetallic(textCoord), 1.0);
}