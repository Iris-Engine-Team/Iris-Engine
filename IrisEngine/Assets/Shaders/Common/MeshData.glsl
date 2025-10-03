#ifndef MESH_DESCRIPTOR
    #define MESH_DESCRIPTOR 1
#endif

layout(set = MESH_DESCRIPTOR, binding = 0) uniform MeshUBO
{
    mat4 Model;
    mat4 NormalMatrix;
}MeshData;
