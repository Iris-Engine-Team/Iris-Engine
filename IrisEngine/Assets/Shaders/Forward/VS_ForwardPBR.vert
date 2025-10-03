#version 450
#extension GL_GOOGLE_include_directive : enable

#include "CameraData.glsl"
#include "MeshData.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextCoord;
layout(location = 3) in vec3 inBiTangent;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec3 normal;
layout(location = 1) out vec2 textCoord;
layout(location = 2) out vec3 vertexWorldPosition;
layout(location = 3) out vec4 cameraPosition;
layout(location = 4) out mat3 TBN;

void main()
{
    textCoord = inTextCoord;

    mat3 inverseModel = mat3(MeshData.NormalMatrix);

    normal = inverseModel * inNormal;

    vec3 T = normalize(inverseModel * inTangent);
    vec3 B = normalize(inverseModel * inBiTangent);
	vec3 N = normalize(normal);

    TBN = mat3(T, B, N);

    cameraPosition = CameraData.CameraPosition;
    vertexWorldPosition = vec3(MeshData.Model * vec4(inPosition, 1.0));

    vec4 position =  MeshData.Model * vec4(inPosition, 1.0);

    gl_Position = (CameraData.ProjectionView) * position;
}