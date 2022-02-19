#pragma once

#include <string>
#include <list>
#include <chrono>
#include <ostream>

class Logger
{
public:
	enum class Severity {
		INFO,
		WARNING,
		ERROR
	};

	void info(const std::wstring&);
	void error(const std::wstring&);
	void warning(const std::wstring&);

	void output(std::wostream& stream) const;

private:
	using time_point = std::chrono::time_point<std::chrono::system_clock>;

	std::list<std::tuple<time_point, Severity, std::wstring>> _messages;
};