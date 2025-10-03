#pragma once

#include "Vector/Vector4.h"
#include "Matrix/Matrix4.h"

namespace Iris
{
	namespace Engine
	{

#define IE_MAX_DIRECTIONAL_LIGHTS   2
#define IE_MAX_SPOT_LIGHTS          20
#define IE_MAX_POINT_LIGHTS         20

#define IE_MAX_DIRECTIONAL_LIGHTS_SHADOWS   1
#define IE_MAX_SPOT_LIGHTS_SHADOWS          8
#define IE_MAX_POINT_LIGHTS_SHADOWS         4

		/// <summary>
		/// Directional light data structure for graphics purposes
		/// </summary>
		struct DirectionalLightData
		{
			Vector4 Direction = Vector4(0.f);
			Vector4 Color = Vector4(0.f);

			float Insensity = 0.f;

			float LightNear = 0.f;
			float LightFar = 0.f;

			bool CalculateShadow = false;

			Matrix4 LightSpaceMatrix = Matrix4::Identity;
		};

		/// <summary>
		/// Point light data structure for graphics purposes
		/// </summary>
		struct PointLightData
		{
			Vector4 Position = Vector4(0.f);
			Vector4 Color = Vector4(0.f);

			float Insensity = 0.f;

			float LightNear = 0.f;
			float LightFar = 0.f;

			bool CalculateShadow = false;
		};

		/// <summary>
		/// Spot light data structure for graphics purposes
		/// </summary>
		struct SpotLightData
		{
			Vector4 Position = Vector4(0.f);
			Vector4 Direction = Vector4(0.f);
			Vector4 Color = Vector4(0.f);

			float Insensity = 0.f;

			float LightNear = 0.f;
			float LightFar = 0.f;

			float CutOff = 0.f;
			float OuterCutOff = 0.f;

			bool CalculateShadow = false;

			float Padding0 = 0.f;
			float Padding1 = 0.f;

			Matrix4 LightSpaceMatrix = Matrix4::Identity;
		};

		/// <summary>
		/// Scene light data struct for graphics purposes
		/// </summary>
		struct SceneLightData
		{
			DirectionalLightData	DirectionalLight[IE_MAX_DIRECTIONAL_LIGHTS];
			SpotLightData			SpotLight[IE_MAX_POINT_LIGHTS];
			PointLightData			PointlLight[IE_MAX_SPOT_LIGHTS];

			int DirLightCount;
			int SpotLightCount;
			int PointLightCount;

			float Padding;
		};
	}
}