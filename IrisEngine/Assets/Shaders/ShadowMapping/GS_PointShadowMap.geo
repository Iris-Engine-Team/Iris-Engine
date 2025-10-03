#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

layout(location = 0) out vec4 FragPos;
layout(location = 1) out vec4 LPos;
layout(location = 2) out float LFar;

layout(set = 0, binding = 0) uniform LightInfo
{
    mat4 ProjectionViews[6];
	vec4 LightPosition;
	float Far;
}LightUBO;

void main()
{
	for(int face = 0; face < 6; ++face)
	{
		gl_Layer = face;
		for(int i = 0; i < 3; ++i)
		{
			FragPos = gl_in[i].gl_Position;
			LPos = LightUBO.LightPosition;
			LFar = LightUBO.Far;

			gl_Position = LightUBO.ProjectionViews[face] * FragPos;

			EmitVertex();
		}

		EndPrimitive();
	}
}