#version 450

layout(location = 0) out vec4 outColor;

layout(location = 1) in vec3 nearPoint;
layout(location = 2) in vec3 farPoint;
layout(location = 3) in mat4 fragView;
layout(location = 7) in mat4 fragProj;

float near = 0.01;
float far = 100.0;

vec4 Grid(vec3 _fragPos3D, float _scale)
{
	vec2 coord = _fragPos3D.xz * _scale;

	vec2 derivative = fwidth(coord);
	vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;

	float line = min(grid.x, grid.y);

	float minimumz = min(derivative.y, 1.0);
	float minimumx = min(derivative.x, 1.0);

	vec4 color = vec4(0.05, 0.05, 0.05, 1.0 - min(line, 1.0));

	if(_fragPos3D.x > -0.1 * minimumx && _fragPos3D.x < 0.1 * minimumx)
		color.z = 0.3;

	if(_fragPos3D.z > -0.1 * minimumz && _fragPos3D.z < 0.1 * minimumz)
        color.x = 0.3;

    return color;
}

float computeDepth(vec3 _pos) 
{
    vec4 clipSpacePos = fragProj * fragView * vec4(_pos.xyz, 1.0);
    return (clipSpacePos.z / clipSpacePos.w);
}

float computeLinearDepth(vec3 _pos) 
{
    vec4 clipSpacePos = fragProj * fragView * vec4(_pos.xyz, 1.0);

    float clipSpaceDepth = (clipSpacePos.z / clipSpacePos.w) * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - clipSpaceDepth * (far - near));

    return linearDepth / far;
}

void main()
{
	float t = -nearPoint.y / (farPoint.y - nearPoint.y);
	vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

	gl_FragDepth = computeDepth(fragPos3D);

	float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

	outColor = (Grid(fragPos3D, 10.0) + Grid(fragPos3D, 1.0)) * float(t > 0);
	outColor.a *= fading;
}