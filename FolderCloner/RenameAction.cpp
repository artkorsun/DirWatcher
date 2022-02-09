#include "RenameAction.h"
#include "Utils.h"

RenameAction::RenameAction(std::filesystem::path source, std::filesystem::path target)
	:	_source(std::move(source)), 
		_target(std::move(target))
{
}

void RenameAction::execute(Logger& logger)
{
	std::error_code ec;
	std::filesystem::rename(_source, _target, ec);

	if (!ec)
	{
		std::wstring log_message = L"File " + _source.wstring() + L" has been renamed to " + _target.wstring();
		logger.log(log_message);
	}
	else
	{
		std::wstring error_message = getLastError();
		logger.log(error_message);
	}
}
