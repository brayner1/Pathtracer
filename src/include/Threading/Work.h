#ifndef PTracer_Work_h
#define PTracer_Work_h
#include <mutex>
#include <functional>

namespace Renderer::threading
{
	class Work
	{
	public:
		Work(bool multi_threaded) : m_dividibleInChunks(multi_threaded), m_finished(false) { }

		virtual ~Work() = default;

		virtual void Execute() = 0;

		virtual bool WillFinishOnNextExecute() = 0;
		/**
		 * \brief Works that are multi-threaded can be divided in chunks and computed by multiple threads in the ThreadPool.
		 */
		[[nodiscard]] bool MultiThreadedWork() const { return m_dividibleInChunks; }

		[[nodiscard]] bool Finished() const { return m_finished; };

		//TODO: Add Wait function  (Maybe 'WaitStart', 'WaitFinish', ... ?)

	protected:
		const bool m_dividibleInChunks;
		std::atomic_bool m_finished;
	};

	template<typename... Args>
	class FunctionWork final : public Work
	{
		using Func = std::function<void(Args...)>;
	public:
		explicit FunctionWork(Func&& function, Args&&... args)
			: Work(false), function(std::forward<Func>(function)), args{std::forward<Args>(args)...}
		{}

		void Execute() override
		{
			std::apply(function, args);

			m_finished = true;
		}

		bool WillFinishOnNextExecute() override
		{
			return true;
		}

		~FunctionWork() override = default;

	private:
		Func function;
		std::tuple<Args...> args;
	};

	template<typename T>
	class ForLoopWork final : public Work
	{
	public:
		using iterator = T;
		using LoopFunc = std::function<void(iterator)>;

		ForLoopWork(iterator end_inter, size_t chunk_size, LoopFunc&& loop_func)
		: ForLoopWork(0, end_inter, chunk_size, std::forward<LoopFunc>(loop_func)) {}

		ForLoopWork(iterator start_iter, iterator end_iter, size_t chunk_size, LoopFunc&& loop_func)
		: Work((end_iter - start_iter) > chunk_size),
		m_currentIter(start_iter), m_endIter(end_iter), m_loopFunc(std::move(loop_func)),
		m_chunkSize(std::min(chunk_size, end_iter - start_iter)), m_numThreads(0)
		{}

		void Execute() override
		{
			std::unique_lock<std::mutex> lock (loopMutex);
			while (!Finished())
			{
				iterator startIt = m_currentIter;
				iterator endIt = std::min(m_currentIter + m_chunkSize, m_endIter);
				m_currentIter = endIt;

				m_numThreads++;

				lock.unlock();

				for (iterator it = startIt; it < endIt; ++it)
				{
					m_loopFunc(it);
				}

				lock.lock();

				m_numThreads--;

				if (endIt == m_endIter)
					m_finished = true;
			}
		}

		bool WillFinishOnNextExecute() override
		{
			size_t currIt;
			{
				std::scoped_lock<std::mutex> lock (loopMutex);
				currIt = m_currentIter;
			}
			return std::min(currIt + m_chunkSize, m_endIter) == m_endIter;
		}

	private:
		iterator m_currentIter;
		const iterator m_endIter;
		const size_t m_chunkSize;
		size_t m_numThreads;
		std::mutex loopMutex;
		LoopFunc m_loopFunc;
	};
}

#endif