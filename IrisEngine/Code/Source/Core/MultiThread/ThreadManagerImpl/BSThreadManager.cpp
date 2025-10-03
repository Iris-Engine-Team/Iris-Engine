#include "Core/MultiThread/ThreadManagerImpl/BSThreadPool.h"
namespace Iris
{
	namespace Engine
	{
		BSThreadPool::BSThreadPool(size_t _nbThread)
		{
			m_Pool = new BS::thread_pool(_nbThread);
		}

		BSThreadPool::~BSThreadPool()
		{
			m_Pool->wait();
			delete m_Pool;
		}

		void BSThreadPool::SubmitTask(std::function<void()> _function)
		{
			m_Pool->detach_task(_function);
		}

		std::vector<std::future<void>> BSThreadPool::SubmitLoop(size_t _start, size_t _end, std::function<void(size_t)>& _func, bool _wait)
		{
			BS::multi_future<void> multi = m_Pool->submit_loop(_start, _end, _func);

			if (_wait)
				multi.wait();

			return multi;
		}

	}
}