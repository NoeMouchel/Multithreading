#include <Core/ThreadPool.hpp>
#include <Core/TimeManager.h>
#include <Core/Log.hpp>
#include <Config.hpp>
#include <iostream>

#include <iomanip>
#include <sstream>

#ifdef MULTITHREAD_ENABLED

Core::ThreadPool::ThreadPool()
{
	m_threadCount = std::thread::hardware_concurrency();
	Core::Log* _log = Core::Log::instance();
	_log->writeThread("Thread count : " + std::to_string(m_threadCount));

	init();
}


Core::ThreadPool::~ThreadPool()
{
	stop();
}


void Core::ThreadPool::init()
{
	for (int i = 0; i < m_threadCount; i++)
	{
		m_worker.push_back(std::thread(&Core::ThreadPool::workerThread, this));
	}
}


void Core::ThreadPool::reset()
{
	stop();
	init();
}


void Core::ThreadPool::stop()
{
	m_stop = true;

	for (std::thread& t : m_worker)
	{
		t.join();
	}

	while (m_flag.test_and_set());

	while (m_queue.empty() == false)
	{
		m_queue.pop();
	}

	m_flag.clear();
}



bool Core::ThreadPool::isWorking()
{
	return !((m_workingThread == 0) && m_queueEmpty);
}

void Core::ThreadPool::addInQueue(std::function<void()> in_method)
{
	while (m_flag.test_and_set());
	m_queue.push(in_method);
	m_flag.clear();

	if (m_queueEmpty)
	{
		Core::Log::instance()->writeThread(" The queue is not empty for now");
		m_queueEmpty = false;
	}
}


void Core::ThreadPool::workerThread()
{
	while (!m_stop)
	{
		//	Lock to avoid multiple thread writing or reading the queue at the same time
		while (m_flag.test_and_set())
		{
			if (m_stop) return;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		if (m_queue.empty() == false)
		{

			std::function<void()> task = m_queue.front();

			m_queue.pop();

			if (m_queue.empty())
			{
				Core::Log::instance()->writeThread(" There is no functon in the queue anymore");
				m_queueEmpty = true;
			}

			//	Release lock 
			m_flag.clear();

			//	Add one in the working thread count
			m_workingThread += 1;

			//	Execute the task
			task();

			//	remove one in the working thread count
			m_workingThread -= 1;

			if (m_workingThread <= 0)
			{
				Core::Log* _log = Core::Log::instance();
				_log->writeThread(" There is no thread working anymore");

				if (sceneLoaderFunctionInQueue)
				{
					float time = Core::TimeManager::instance()->getTime() - sceneStartLoadTime;

					std::stringstream stream;
					stream << std::fixed << std::setprecision(2) << time;

					_log->writeThread(" The scene successfully loaded (" + stream.str() + " sec)", C_PINK);
					sceneLoaderFunctionInQueue = false;
				}
			}
		}
		else
		{
			std::this_thread::yield();

			m_flag.clear();
		}
	}
}

#endif

void Core::lockFlag(std::atomic_flag& flag)
{
#ifdef MULTITHREAD_ENABLED
	while (flag.test_and_set());
#endif

	return;
}

void Core::unlockFlag(std::atomic_flag& flag)
{
#ifdef MULTITHREAD_ENABLED
	flag.clear();
#endif

	return;
}
