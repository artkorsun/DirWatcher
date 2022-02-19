#include "Logger.h"

#include <iostream>
#include <cwchar>
#include <locale>
#include <codecvt>

#include <map>

namespace
{
std::wstring severity_str(Logger::Severity severity)
{
	static std::map<Logger::Severity, std::wstring> severity_to_str = { {Logger::Severity::INFO, L"INFO"},
		                                                                {Logger::Severity::WARNING, L"WARNING"},
																		{Logger::Severity::ERROR, L"ERROR"}};

	return severity_to_str[severity];
}
}

void Logger::output(std::wostream& stream) const
{
	for (const auto& log_entry : _messages) {
		const std::time_t t_c = std::chrono::system_clock::to_time_t(std::get<0>(log_entry));
		
		wchar_t wstr[100];
		if (std::wcsftime(wstr, 100, L"%F %T", std::localtime(&t_c))) {
			stream << wstr;
		}
		else {
			stream << L"N/A";
		}

		stream << " [" << severity_str(std::get<1>(log_entry)) << "]\t";
		stream << std::get<2>(log_entry) << "\n";
	}
}

void Logger::info(const std::wstring& message)
{
	_messages.push_back({ std::chrono::system_clock::now(), Severity::INFO, message });
}

void Logger::error(const std::wstring& message)
{
	_messages.push_back({ std::chrono::system_clock::now(), Severity::ERROR, message });
}

void Logger::warning(const std::wstring& message)
{
	_messages.push_back({ std::chrono::system_clock::now(), Severity::WARNING, message });
}
