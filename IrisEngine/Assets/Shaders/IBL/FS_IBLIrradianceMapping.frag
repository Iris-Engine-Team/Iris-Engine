#version 450

layout(location = 0) in vec3 LocalPos;

layout(location = 0) out vec4 FragColor;

layout(set = 1, binding = 0) uniform samplerCube SkyboxTexture;

const float PI = 3.14159265359;

void main()
{
	vec3 normal = normalize(LocalPos);
	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal, right));

	// Increase or decrease the accuracy of the irradiance
	float sampleDelta = 0.025;
	float nrSamples = 0.0;

	// Traverse the hemisphere 
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		// First loop is running arround the equator so between 0 and 2 PI

		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// Second loop is running arround the equator and north pole of the hemisphere so between 0 and PI/2

			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += texture(SkyboxTexture, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0 / nrSamples);

	FragColor = vec4(irradiance, 1.0);
}
