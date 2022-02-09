#pragma once

#include <string>
#include <list>

class Logger
{
public:
	void log(const std::wstring&);

private:
	std::list<std::wstring> _messages;
};