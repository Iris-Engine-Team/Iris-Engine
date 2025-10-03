#include "Core/Time.h"
#include "Core/IWindow.h"

namespace Iris
{
	namespace Engine
	{
		double Time::GetFixedDeltaTimeCounterD()
		{
			if (m_FixedDeltaTimeCounter > m_FixedDeltaTime)
				return m_FixedDeltaTime;

			return m_FixedDeltaTimeCounter;
		}

		void Time::UpdateDeltaTime(IWindow* _window)
		{
			double current = static_cast<float>(_window->GetTime());
			m_DeltaTime = current - m_LastFrameTime;
			m_LastFrameTime = current;
		}

		bool Time::UpdateFixedDeltaTime()
		{
			bool returnValue = m_FixedDeltaTimeCounter >= m_FixedDeltaTime;

			if(returnValue)
				m_FixedDeltaTimeCounter -= m_FixedDeltaTime;

			return returnValue;
		}

		void Time::IncrementFixedDeltaTime()
		{
			m_FixedDeltaTimeCounter += static_cast<float>(m_DeltaTime);
		}
	}
}