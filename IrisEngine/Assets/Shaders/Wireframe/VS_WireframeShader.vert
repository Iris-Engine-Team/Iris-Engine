#version 450
#extension GL_GOOGLE_include_directive : enable

#include "CameraData.glsl"

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 wireFrameColor;

layout(push_constant) uniform WireframeUBO
{
	mat4 Model;
	vec4 Color;
}WireframeData;

void main()
{
	wireFrameColor = WireframeData.Color;

	vec4 position =  WireframeData.Model * vec4(inPosition, 1.0);
	gl_Position = (CameraData.ProjectionView) * position;
}