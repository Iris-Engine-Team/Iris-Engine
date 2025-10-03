#version 450

layout(location = 0) in vec3 inPosition;

layout(location = 0) out mat4[6] ProjectionViews;

layout(push_constant) uniform MeshInfo
{
    mat4 Model;
    mat4 NormalMatrix;
}MeshUBO;

void main()
{
	gl_Position = MeshUBO.Model * vec4(inPosition, 1.0);
}