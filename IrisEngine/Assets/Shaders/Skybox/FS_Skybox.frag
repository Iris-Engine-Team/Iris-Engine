#version 460

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 TextCoord;

layout(set = 1, binding = 0) uniform samplerCube SkyboxTexture;

void main()
{
	FragColor = texture(SkyboxTexture, TextCoord);
}
