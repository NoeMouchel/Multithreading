#pragma once
/*
#include <iostream>
#include <thread>
#include <semaphore>
#include <deque>

//	Inspired from https://stackoverflow.com/questions/36762248/why-is-stdqueue-not-thread-safe

template<typename T>
class SharedQueue
{
private:

	//	Private Internal Variables
	//	--------------------------
	
	std::deque<T> m_queue;

	std::counting

public:

	//	Constructors / Destructors
	//	--------------------------

	SharedQueue();
	~SharedQueue();


	//	Public Internal Variables
	//	-------------------------

	T& front();
	void pop_front();

	void push_back(const T& element);
	void push_back(const T&& element);

	int size();
	bool empty();
};*/