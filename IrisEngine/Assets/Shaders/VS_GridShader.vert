#version 450

layout(location = 1) out vec3 nearPoint;
layout(location = 2) out vec3 farPoint;
layout(location = 3) out mat4 fragView;
layout(location = 7) out mat4 fragProj;

layout(push_constant) uniform CameraUBO
{
    mat4 View;
    mat4 Projection;
}CameraData;

vec3 gridPlane[6] = vec3[](
    vec3(1.0, 1.0, 0.0), vec3(-1.0, -1.0, 0.0), vec3(-1.0, 1.0, 0.0),
    vec3(-1.0, -1.0, 0.0), vec3(1.0, 1.0, 0.0), vec3(1.0, -1.0, 0.0)
);

vec3 UnprojectedPoint(float _x, float _y, float _z, mat4 _view, mat4 _projection)
{
    mat4 viewInv = inverse(_view);
    mat4 projInv = inverse(_projection);

    vec4 unprojectedPoint = viewInv * projInv * vec4(_x, _y, _z, 1.0);
    
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() 
{
    vec3 point = gridPlane[gl_VertexIndex].xyz;
    
    fragView = CameraData.View;
    fragProj = CameraData.Projection;

    nearPoint = UnprojectedPoint(point.x, point.y, 0.0, CameraData.View, CameraData.Projection).xyz;
    farPoint = UnprojectedPoint(point.x, point.y, 1.0, CameraData.View, CameraData.Projection).xyz;

    gl_Position = vec4(point, 1.0);
}