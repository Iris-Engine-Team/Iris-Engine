#version 450

vec3 PlanePositions[6] = vec3[](
    vec3(1.0, 1.0, 0.0),
    vec3(-1.0, -1.0, 0.0),
    vec3(-1.0, 1.0, 0.0),
    vec3(-1.0, -1.0, 0.0),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, -1.0, 0.0)
);

vec2 PlaneUVs[6] = vec2[](
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0) 
);

layout(location = 0) out vec2 TextCoords;

void main()
{
	TextCoords = PlaneUVs[gl_VertexIndex];
	gl_Position = vec4(PlanePositions[gl_VertexIndex], 1.0);
}
