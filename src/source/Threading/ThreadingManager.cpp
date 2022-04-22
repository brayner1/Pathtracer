#include "pch.h"
#include "Threading/ThreadingManager.h"
#include "Threading/Barrier.h"

namespace Renderer::threading
{

	static std::mutex coutMutex;

	ThreadingManager& ThreadingManager::Get()
	{
		static ThreadingManager manager;
		return manager;
	}

	void ThreadingManager::DisableWorkerThreads()
	{
		if (endWorkers)
			return;
		endWorkers = true;
		for (std::thread& thread : threadPool)
			thread.join();
	}

	void ThreadingManager::EnableWorkerThreads()
	{
		if (!endWorkers)
			return;

		endWorkers = false;
		for (std::thread& thread : threadPool)
			thread.join();
	}

	ThreadingManager::~ThreadingManager()
	{
		// Guarantee that if the manager is destructed before disabling the threads, the threads are joined.
		if (!endWorkers)
		{
			endWorkers = true;
			for (std::thread& thread : threadPool)
				thread.join();
		}
	}

	ThreadingManager::ThreadingManager()
	{
		threadPool = std::vector<std::thread>{};
		uint32_t numThreads = std::thread::hardware_concurrency();
		threadPool.reserve(numThreads);

		//Barrier initBarrier(numThreads);
		std::shared_ptr<Barrier> initBarrier = std::make_shared<Barrier>(numThreads);

		for (int i = 0; i < numThreads - 1; i++)
		{
			threadPool.push_back(std::thread{&ThreadingManager::ThreadWorker, this, i + 1, initBarrier});
			//threadPool.emplace_back(ThreadWorker, i + 1, initBarrier);
		}

		std::cout << "waiting main thread\n";

		initBarrier->Wait();

		std::cout << "finishing main thread\n";
	}

	
	// Dando problema pq n é static!
	void ThreadingManager::ThreadWorker(int thread_id, std::shared_ptr<Barrier> barrier)
	{

		barrier->Wait();

		while (!endWorkers)
		{
			
		}
	}

	/*void ThreadWorker(int thread_id)
	{
	}*/
}
