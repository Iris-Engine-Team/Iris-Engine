#version 450
#extension GL_GOOGLE_include_directive : enable

layout(location = 0) out vec3 cameraPosition;

#include "CameraData.glsl"

vec3 plane[6] = vec3[](
    vec3(1.0, 1.0, 0.0), vec3(-1.0, -1.0, 0.0), vec3(-1.0, 1.0, 0.0),
    vec3(-1.0, -1.0, 0.0), vec3(1.0, 1.0, 0.0), vec3(1.0, -1.0, 0.0)
);

void main() 
{
    vec3 point = plane[gl_VertexIndex].xyz;

    cameraPosition = vec3(CameraData.CameraPosition);

    gl_Position = vec4(point, 1.0);
}