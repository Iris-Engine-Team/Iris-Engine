#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextCoord;
layout(location = 3) in vec2 inBiTangent;
layout(location = 4) in vec2 inTangent;

layout(location = 0) out vec2 textCoord;

layout(set = 0, binding = 0) uniform CameraInfo
{
    mat4 View;
    mat4 Projection;
}CamUBO;

layout(set = 1, binding = 0) uniform MeshInfo
{
    mat4 Model;
}MeshUBO;

void main() 
{
   textCoord = inTextCoord; 
   gl_Position = (CamUBO.Projection * CamUBO.View) * MeshUBO.Model * vec4(inPosition, 1.0);
}