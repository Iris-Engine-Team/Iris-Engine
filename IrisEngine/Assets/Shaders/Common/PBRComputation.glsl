const float PI = 3.141592;

///////////////////////////////////////////////////////////////////////

/// PBR functions

///////////////////////////////////////////////////////////////////////

float D_GGX(float _DotNH, float _PRoughness)
{
    float a2 = _PRoughness * _PRoughness;
    float f = (_DotNH * a2 - _DotNH) * _DotNH + 1.0;

    return a2 / (PI * f * f);
}

float V_SmithGGXCorrelated(float _DotNV, float _DotNL, float _PRoughness)
{
	float a2 = _PRoughness * _PRoughness;
	float GGXV = _DotNL * sqrt(_DotNV * _DotNV * (1.0 - a2) + a2);
	float GGXL = _DotNV * sqrt(_DotNL * _DotNL * (1.0 - a2) + a2);

	return 0.5 / (GGXV + GGXL);
}

vec3 F_Schlick(float _DotVH, vec3 _F0)
{
	 return _F0 + (vec3(1.0) - _F0) * pow(1.0 - _DotVH, 5.0);
}

vec3 F_SchlickRoughness(float _DotVH, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - _DotVH, 0.0, 1.0), 5.0);
}   

float Fd_Lambert()
{
	return 1.0 / PI;
}

// optimized V function
float V_SmithGGXCorrelatedFast(float _DotNV, float _DotNL, float _PRoughness)
{
	float GGXV = _DotNL * (_DotNV * (1.0 - _PRoughness) + _PRoughness);
	float GGXL = _DotNV * (_DotNL * (1.0 - _PRoughness) + _PRoughness);

	return 0.5 / (GGXV + GGXL);
}

// Disney Fd function
float F_SchlickForBurley(float _DotVH, float _F0, float _F90)
{
    return _F0 + (_F90 - _F0) * pow(1.0 - _DotVH, 5.0);
}

float Fd_Burley(float _DotNV, float _DotNL, float _DotLH, float _PRoughness)
{
    float f90 = 0.5 + 2.0 * _PRoughness * _DotLH * _DotLH;
    float lightScatter = F_SchlickForBurley(_DotNL, 1.0, f90);
    float viewScatter = F_SchlickForBurley(_DotNV, 1.0, f90);

    return lightScatter * viewScatter * (1.0 / PI);
}

vec2 hammersley(uint i, float numSamples)
{
    uint bits = i;
    bits = (bits << 16) | (bits >> 16);
    bits = ((bits & 0x55555555) << 1) | ((bits & 0xAAAAAAAA) >> 1);
    bits = ((bits & 0x33333333) << 2) | ((bits & 0xCCCCCCCC) >> 2);
    bits = ((bits & 0x0F0F0F0F) << 4) | ((bits & 0xF0F0F0F0) >> 4);
    bits = ((bits & 0x00FF00FF) << 8) | ((bits & 0xFF00FF00) >> 8);
    return vec2(i / numSamples, bits / exp2(32));
}

vec3 importanceSampleGGX(vec2 Xi, float roughness, vec3 N)
{
    float a = roughness * roughness;

    // Mapping Xi.x and Xi.y to spherical coordinates (GGX distribution)
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // Spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = sinTheta * cos(phi);
    H.y = sinTheta * sin(phi);
    H.z = cosTheta;

    // Transform the normal vector from tangent space to world space
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    // Convert H from tangent space to world space
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float GDFG(float _DotNV, float _DotNL, float _A)
{
    float a2 = _A * _A;
    float GGXL = _DotNV * sqrt((-_DotNL * a2 + _DotNL) * _DotNL + a2);
    float GGXV = _DotNL * sqrt((-_DotNV * a2 + _DotNV) * _DotNV + a2);
    return (2 * _DotNL) / (GGXV + GGXL);
}

vec2 DFG(float NoV, float a, vec3 n) {
    vec3 V;
    V.x = sqrt(1.0f - NoV*NoV);
    V.y = 0.0f;
    V.z = NoV;

    vec2 r = vec2(0.0f);
    for (uint i = 0; i < 256; i++) {
        vec2 Xi = hammersley(i, 256);
        vec3 H = importanceSampleGGX(Xi, a, normalize(n));
        vec3 L = 2.0f * dot(V, H) * H - V;

        float VoH = clamp(dot(V, H), 0, 1);
        float NoL = clamp(L.z, 0, 1);
        float NoH = clamp(H.z, 0, 1);

        if (NoL > 0.0f) {
            float G = GDFG(NoV, NoL, a);
            float Gv = G * VoH / NoH;
            float Fc = pow(1 - VoH, 5.0f);
            r.x += Gv * (1 - Fc);
            r.y += Gv * Fc;
        }
    }
    return r * (1.0f / 256);
}

vec3 BRDF(vec3 _Vue, vec3 _Normal, vec3 _Light, float _PerceptualRoughness, float _Metalness, vec3 _TextColor, vec3 _Reflectance, float _shadowValue)
{
	vec3 _F0 = vec3(0.16 * (_Reflectance * _Reflectance));
	_F0 = mix(_F0, _TextColor, _Metalness);

    vec3 halfUnitVec = normalize(_Vue + _Light);

    float dotNV = abs(dot(_Normal, _Vue)) + 1e-5;
    float dotNL = clamp(dot(_Normal, _Light), 0.001, 1.0);
    float dotNH = clamp(dot(_Normal, halfUnitVec), 0.0, 1.0);
    float dotVH = clamp(dot(_Vue, halfUnitVec), 0.0, 1.0);

    // perceptually linear roughness to roughness (see parameterization)
    float roughness = _PerceptualRoughness * _PerceptualRoughness;

    float D = D_GGX(dotNH, roughness);
    vec3  F = F_Schlick(dotVH, _F0);
    float V = V_SmithGGXCorrelatedFast(dotNV, dotNL, roughness);
	//vec3 energyCompensation = 1.0 + _F0 * (1.0 / DFG(dotNV, roughness).y - 1.0);

    // specular BRDF
    vec3 Fr = D * F * V;
	//Fr *= energyCompensation;

    // diffuse BRDF
    vec3 Fd = (1.0 - _Metalness) * _TextColor * Fd_Lambert() * _shadowValue;

	return Fr + Fd;
}