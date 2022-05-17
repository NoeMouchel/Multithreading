#pragma once

#include <string>
#include <atomic>

#include "Utils/Singleton.h"

namespace Core
{
	class Log : public Singleton<Log>
	{
	private:
		//	Private Internal Variables
		//	-------------------------

		//	Log file name
		std::string m_logName;
		std::string m_threadLogName;

		//	Content written in log during its creation
		std::string m_logContent;
		std::string m_threadLogContent;

		//	Write the string in the log content
		//	Parameters : string in_line
		//	---------------------------
		void writeLog(const std::string& in_line, const int color);


		//	Write the string in the thread log content
		//	Parameters : string in_line
		//	---------------------------
		void writeThreadLog(const std::string& in_line, const int color);

	public:
		//	Constructor & Destructor
		//	------------------------

		Log();

		~Log();


		//	Public Internal Functions
		//	-------------------------


		//	Log Content
		

		//	Write the string in the log content
		//	Parameters : string in_line
		//	---------------------------
		void write(const std::string& in_line);

		//	Write the string in the log content
		//	Parameters : string in_line, const int color
		//	--------------------------------------------
		void write(const std::string& in_line, const int color);

		//	Write a Success line in the log content
		//	Parameters : string in_line
		//	---------------------------
		void writeSuccess(const std::string& in_line);

		//	Write a Failure line in the log content
		//	Parameters : string in_line
		//	---------------------------
		void writeFailure(const std::string& in_line);

		//	Write a Waring line in the log content
		//	Parameters : string in_line
		//	---------------------------
		void writeWarning(const std::string& in_line);

		//	Write an Error line in the log content
		//	Parameters : string in_line
		//	---------------------------
		void writeError(const std::string& in_line);


		//	Thread Log Content


		//	Write the string in the thread log content
		//	Parameters : string in_line
		//	---------------------------
		void writeThread(const std::string& in_line);

		//	Write the string in the thread log content
		//	Parameters : string in_line, int color
		//	--------------------------------------
		void writeThread(const std::string& in_line, int color);

		//	Write a Success line in the thread log content
		//	Parameters : string in_line
		//	---------------------------
		void writeThreadSuccess(const std::string& in_line);

		//	Write a Failure line in the thread log content
		//	Parameters : string in_line
		//	---------------------------
		void writeThreadFailure(const std::string& in_line);

		//	Write a Waring line in the thread log content
		//	Parameters : string in_line
		//	---------------------------
		void writeThreadWarning(const std::string& in_line);

		//	Write an Error line in the thread log content
		//	Parameters : string in_line
		//	---------------------------
		void writeThreadError(const std::string& in_line);


		//	Break a line
		//	Parameters : none
		//	-----------------
		void breakLine();
	};
}