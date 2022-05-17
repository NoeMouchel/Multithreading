/*#include <Utils/SharedQueue.h>


template<typename T>
SharedQueue<T>::SharedQueue()
{}


template<typename T>
SharedQueue<T>::~SharedQueue()
{}


template<typename T>
T& SharedQueue<T>::front()
{
	//	Do Semaphore lock here

	while (m_queue.empty())
	{

	}

	return m_queue.front();
}

template<typename T>
bool SharedQueue<T>::empty()
{
	return m_queue.empty();
}


template <typename T>
void SharedQueue<T>::pop_front()
{
	//	Do Semaphore lock here
	while (queue_.empty())
	{

	}
	m_queue.pop_front();
}

template <typename T>
void SharedQueue<T>::push_back(const T& element)
{
	m_queue.push_back(element);
}


template <typename T>
void SharedQueue<T>::push_back(const T&& element)
{
	m_queue.push_back(element);
}

template <typename T>
int SharedQueue<T>::size()
{
	return m_queue.size();
}*/
