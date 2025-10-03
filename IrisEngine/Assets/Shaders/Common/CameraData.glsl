#ifndef CAMERA_DESCRIPTOR
    #define CAMERA_DESCRIPTOR 0
#endif

layout(set = CAMERA_DESCRIPTOR, binding = 0) uniform CameraUBO
{
    mat4 ProjectionView;
    mat4 View;
    mat4 Projection;
    vec4 CameraPosition;
}CameraData;