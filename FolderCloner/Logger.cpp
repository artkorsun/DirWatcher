#include "Logger.h"

#include <iostream>

void Logger::log(const std::wstring& message)
{
	_messages.push_back(message);

	std::wcout << message << std::endl;
}