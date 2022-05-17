#pragma once

#include <windows.h>


class ConsoleHandler
{
private:
	//	Private Variables
	//	-----------------

	HANDLE m_console;

public:

	//	Public Variables
	//	----------------

	int m_defaultColor;
	//	Constructor
	//	-----------

	ConsoleHandler(int defaultColor)
	{
		m_console = GetStdHandle(STD_OUTPUT_HANDLE); 
		m_defaultColor = defaultColor;
	}

	//	Public Functions
	//	----------------

	void setColor(int color);
	void resetColor();
};