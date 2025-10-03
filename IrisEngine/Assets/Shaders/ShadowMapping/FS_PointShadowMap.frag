#version 450

layout(location = 0) in vec4 FragPos;
layout(location = 1) in vec4 LPos;
layout(location = 2) in float LFar;

void main()
{
	float lightDistance = length(FragPos.xyz - LPos.xyz);

	lightDistance = lightDistance / LFar;

	gl_FragDepth = lightDistance;
}