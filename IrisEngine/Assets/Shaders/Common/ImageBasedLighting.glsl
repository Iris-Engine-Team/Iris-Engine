#ifndef IBL_DESCRIPTOR
    #define IBL_DESCRIPTOR 5
#endif

layout(set = IBL_DESCRIPTOR, binding = 0) uniform samplerCube IrradianceMap;
layout(set = IBL_DESCRIPTOR, binding = 1) uniform samplerCube PrefilteredMap;
layout(set = IBL_DESCRIPTOR, binding = 2) uniform sampler2D LookUpMap;