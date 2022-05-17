#include <Utils/ConsoleHandler.h>


void ConsoleHandler::setColor(int color)
{
	SetConsoleTextAttribute(m_console, color);
}


void ConsoleHandler::resetColor()
{
	SetConsoleTextAttribute(m_console, m_defaultColor);
}