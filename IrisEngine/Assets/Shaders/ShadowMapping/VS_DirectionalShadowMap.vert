#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextCoord;
layout(location = 3) in vec3 inBiTangent;
layout(location = 4) in vec3 inTangent;

layout(set = 0, binding = 0) uniform LightInfo
{
    mat4 ProjectionView;
}LightUBO;

layout(push_constant) uniform MeshInfo
{
    mat4 Model;
    mat4 NormalMatrix;
}MeshUBO;

void main()
{
    gl_Position = LightUBO.ProjectionView * MeshUBO.Model * vec4(inPosition, 1.0);
}