#pragma once

#include <Utils/Singleton.h>
#include <deque>
#include <thread>
#include <atomic>
#include <functional>
#include <queue>

#include <stdarg.h>

#define A_LOCKED true
#define A_UNLOCKED false


//	HERE TO DISABLE/ENABLE MULTITHREADING
#define MULTITHREAD_ENABLED

namespace Core
{
#ifdef MULTITHREAD_ENABLED
	class ThreadPool : public Singleton<ThreadPool>
	{
	private:

		//	Private Internal Variables
		//	--------------------------

		int  m_threadCount;
		bool m_queueEmpty = true;
		
		std::queue<std::function<void()>> m_queue;
		std::deque<std::thread> m_worker;

		std::atomic<int> m_workingThread = 0;

		std::atomic_flag m_flag = ATOMIC_FLAG_INIT;

		//	Private Internal Functions
		//	--------------------------

		void workerThread();

	public:
		bool sceneLoaderFunctionInQueue = false;
		float sceneStartLoadTime;

		std::atomic<bool> m_stop = false;

		//	Constructors / Destructors
		//	--------------------------

		ThreadPool();
		~ThreadPool();
				
		
		//	Public Internal Functions
		//	-------------------------

		void addInQueue(std::function<void()> in_method);
		void init();
		void reset();
		void stop();

		bool isWorking();
	};

#endif

	void lockFlag(std::atomic_flag& flag);
	void unlockFlag(std::atomic_flag& flag);
}