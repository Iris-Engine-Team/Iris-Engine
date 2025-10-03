#pragma once

#include "../Externals/ThreadPool/BS_thread_pool.hpp"
#include "Core/Tools/Macros.h"
#include <vector>

namespace Iris
{
	namespace Engine
	{
		class BSThreadPool
		{
		public:
			BSThreadPool(size_t _nbThread);
			~BSThreadPool();

			template <typename F, typename R = std::invoke_result_t<std::decay_t<F>>>
			IE_FORCE_INLINE std::future<R> SubmitTask(F&& _function)
			{
				return m_Pool->submit_task(_function);
			}

			void SubmitTask(std::function<void()> _function);

			void Wait() { m_Pool->wait(); }

			std::vector<std::future<void>> SubmitLoop(size_t _start, size_t _end, std::function<void(size_t)>& _func, bool _wait);
		private:

			BS::thread_pool<BS::tp::none> * m_Pool = nullptr;
		};
	}
}