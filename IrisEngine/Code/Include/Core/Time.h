#pragma once

#include "Core/Tools/Macros.h"
#include "Core/Tools/Log.h"

namespace Iris
{
	namespace Engine
	{
		class IWindow;

		class Time
		{
		public:
			/// <summary>
			/// Gets the current delta time
			/// </summary>
			/// <returns>A double corresponding to the delta time</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE double GetDeltaTimeD() { return m_DeltaTime; }
			
			/// <summary>
			/// Gets the current delta time
			/// </summary>
			/// <returns>A float corresponding to the delta time</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE float GetDeltaTimeF() { return static_cast<float>(m_DeltaTime); }

			/// <summary>
			/// Gets the current fixed delta time
			/// </summary>
			/// <returns>A double corresponding to the fixed delta time</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE double GetFixedDeltaTimeD() { return static_cast<float>(m_FixedDeltaTime); }
			
			/// <summary>
			/// Gets the current fixed delta time
			/// </summary>
			/// <returns>A float corresponding to fixed the delta time</returns>
			IRIS_ENGINE_API IE_FORCE_INLINE float GetFixedDeltaTimeF() { return static_cast<float>(m_FixedDeltaTime); }

			/// <summary>
			/// Set fixed delta time
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API IE_FORCE_INLINE void SetFixedDeltaTime(double _fixedDeltaTime) { m_FixedDeltaTime = _fixedDeltaTime; }

			/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
			IRIS_ENGINE_API double GetFixedDeltaTimeCounterD();

			/// <summary>
			/// Updates the delta time
			/// </summary>
			IRIS_ENGINE_API void UpdateDeltaTime(IWindow* _window);

			/// <summary>
			/// Updates the fixed delta time
			/// </summary>
			/// <returns>A bool corresponding to the possible execution of all fixed functions</returns>
			IRIS_ENGINE_API bool UpdateFixedDeltaTime();

			/// <summary>
			/// Updates the fixed delta time
			/// </summary>
			/// <returns>A bool corresponding to the possible execution of all fixed functions</returns>
			IRIS_ENGINE_API void IncrementFixedDeltaTime();

		private:
			double m_DeltaTime = 0.0;
			double m_FixedDeltaTime = 0.02;

			double m_LastFrameTime = 0.0;
			double m_FixedDeltaTimeCounter = 0.0;
		};
	}
}