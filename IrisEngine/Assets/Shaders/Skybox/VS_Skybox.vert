#version 460
#extension GL_GOOGLE_include_directive : enable

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 TextCoord;

#include "CameraData.glsl"

void main()
{
    TextCoord = inPosition;
    vec4 pos = CameraData.Projection * mat4(mat3(CameraData.View)) * vec4(inPosition, 1.0);
    pos.z = pos.w;
    gl_Position = pos;
}
