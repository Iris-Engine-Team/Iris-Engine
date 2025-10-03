#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

layout(location = 0) in vec3 LocalPos_in[];
layout(location = 0) out vec3 LocalPos;  

layout(set = 0, binding = 0) uniform CubemapUBO
{
	mat4 Projection;
	mat4 Views[6];
}CubemapData;

void main()
{
	for(int face = 0; face < 6; ++face)
	{
		gl_Layer = face;
		for(int i = 0; i < 3; ++i)
		{
			LocalPos = LocalPos_in[i];
			gl_Position = CubemapData.Projection * CubemapData.Views[face] * vec4(LocalPos, 1.0);

			EmitVertex();
		}

		EndPrimitive();
	}
}