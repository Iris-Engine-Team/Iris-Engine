#pragma once

#include "Core/Tools/Macros.h"
#include <future>
#include <functional>

#include "Core/MultiThread/ThreadManagerImpl/BSThreadPool.h"

namespace Iris
{
	namespace Engine
	{
		class ThreadManager
		{
		public:
			ThreadManager(size_t _nbThread);
			~ThreadManager();

			IE_FORCE_INLINE void SubmitTask(std::function<void()> _function) const
			{
				m_Pool->SubmitTask(_function);
			}

			template <typename F, typename R = std::invoke_result_t<std::decay_t<F>>>
			IE_FORCE_INLINE std::future<R> SubmitTask(F&& _function) const
			{
				return m_Pool->SubmitTask(_function);
			}

			IE_FORCE_INLINE  void WaitAllTasks() const { m_Pool->Wait(); }

			std::vector<std::future<void>> SubmitLoop(size_t _start, size_t _end, std::function<void(size_t)> _func, bool _wait = true) const;

			IE_FORCE_INLINE size_t GetNbThread() const { return m_NbThread; }
		private:

			size_t m_NbThread = 0;
			BSThreadPool* m_Pool = nullptr;
		};
	}
}