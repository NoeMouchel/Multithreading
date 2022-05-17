#include <iostream>
#include <ctime>

#include <fstream>
#include <sstream>

#include <Core/Log.hpp>
#include <Utils/consoleHandler.h>
#include <Config.hpp>

#include <Core/ThreadPool.hpp>


#define MAX_S 64

ConsoleHandler console = ConsoleHandler(C_LIGHT_GRAY);
std::atomic_flag m_consoleUsed = ATOMIC_FLAG_INIT;

void getTime(char* c)
{
	//	Set Time
	time_t now = time(0);
	struct tm time;

	#if defined(__unix__)
		localtime_r(&time, &now);
	#elif defined(_MSC_VER)
		localtime_s(&time, &now);
	#endif

	//	Create char array c with a specific time format -> HH:MM:SS
	strftime(c, MAX_S, "%H:%M:%S", &time);
}

Core::Log::Log()
{
	//	Get time
	char c[MAX_S];

	//	Set Time
	time_t now = time(0);
	struct tm time;

	#if defined(__unix__)
		localtime_r(&time, &now);
	#elif defined(_MSC_VER)
		localtime_s(&time, &now);
	#endif

	//	Create char array c with a specific time format -> HH_MM_SS
	strftime(c, MAX_S, "%H_%M_%S", &time);

	//	Set log name
	m_logName = "Resource/Log/Log_" + std::string(c) + ".log";
	m_threadLogName = "Resource/Log/ThreadLog_" + std::string(c) + ".log";
}

Core::Log::~Log()
{
	//	Write its content in a file at he end
	//	-------------------------------------


	//	Log 
	{
		std::ofstream file(m_logName);

		if (file.is_open() == false)
		{
			writeError("Unable to create log file - The log won't be saved");
			return;
		}

		write("Closing program...");

		file << m_logContent;

		file.close();
	}

	#ifdef MULTITHREAD_ENABLED
	//	Thread Log
	{
		std::ofstream file(m_threadLogName);

		if (file.is_open() == false)
		{
			writeError("Unable to create thread log file - The thread log won't be saved");
			return;
		}

		write("Closing program...");

		file << m_threadLogContent;

		file.close();
	}
	#endif
}


std::string getTimeString()
{
	//	Get time
	char c[MAX_S];
	getTime(c);

	std::string time = "[" + std::string(c) + "]" + "\t";
	return time;
}


std::string createLine(const std::string& in_line)
{
	std::string time = getTimeString();

	//	Create new line
	std::string out = time + in_line + "\n" + time + "-------	|\n";

	return out;
}


void printConsole(const std::string& in_line, const int color)
{
	console.setColor(color);

	std::cout << in_line;// +"\n";

	console.resetColor();
}


void Core::Log::writeLog(const std::string& in_line, const int color)
{
	std::string l = createLine(in_line);

	//	Display in console
	printConsole(l, color);

	//	Add in content
	m_logContent += l;
}


void Core::Log::writeThreadLog(const std::string& in_line, const int color)
{
	std::string l = createLine(in_line);

	//	Display in console
	printConsole(l, color);

	//	Add in content
	m_threadLogContent += l;
}


//	log content

void Core::Log::write(const std::string& in_line)
{
	writeLog("INFO    | " + in_line, console.m_defaultColor);
}

void Core::Log::write(const std::string& in_line, const int color)
{
	writeLog("INFO    | " + in_line, color);
}

void Core::Log::writeSuccess(const std::string& in_line)
{
	writeLog("SUCCESS | " + in_line, C_DARK_GREEN);
}

void Core::Log::writeFailure(const std::string& in_line)
{
	writeLog("FAILURE | " + in_line, C_DARK_YELLOW);
}

void Core::Log::writeWarning(const std::string& in_line)
{
	writeLog("WARNING | " + in_line, C_YELLOW);
}

void Core::Log::writeError(const std::string& in_line)
{
	writeLog("ERROR   | " + in_line, C_RED);
}

//	Thread log content

void Core::Log::writeThread(const std::string& in_line)
{
	writeThreadLog("INFO    | " + in_line, C_CYAN);
}

void Core::Log::writeThread(const std::string& in_line, int color)
{
	writeThreadLog("INFO    | " + in_line, color);
}

void Core::Log::writeThreadSuccess(const std::string& in_line)
{
	writeThreadLog("SUCCESS | " + in_line, C_GREEN);
}

void Core::Log::writeThreadFailure(const std::string& in_line)
{
	writeThreadLog("FAILURE | " + in_line, C_DARK_YELLOW);
}

void Core::Log::writeThreadWarning(const std::string& in_line)
{
	writeThreadLog("WARNING | " + in_line, C_DARK_YELLOW);
}

void Core::Log::writeThreadError(const std::string& in_line)
{
	writeThreadLog("ERROR   | " + in_line, C_RED);
}



void Core::Log::breakLine()
{
	writeLog("-------	|", console.m_defaultColor);
}