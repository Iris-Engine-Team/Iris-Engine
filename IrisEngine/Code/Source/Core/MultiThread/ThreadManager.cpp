#include "Core/MultiThread/ThreadManager.h"

namespace Iris
{
	namespace Engine
	{
		ThreadManager::ThreadManager(size_t _nbThread)
		{
			m_NbThread = _nbThread;
			m_Pool = new BSThreadPool(_nbThread);
		}

		ThreadManager::~ThreadManager()
		{
			delete m_Pool;
		}

		std::vector<std::future<void>> ThreadManager::SubmitLoop(size_t _start, size_t _end, std::function<void(size_t)> _func, bool _wait) const
		{
			return m_Pool->SubmitLoop(_start, _end, _func, _wait);
		}
	}
}