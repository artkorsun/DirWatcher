#include "DeleteAction.h"
#include "Utils.h"

DeleteAction::DeleteAction(std::filesystem::path target)
	:	_target(std::move(target))
{
}

void DeleteAction::execute(Logger& logger)
{
	std::error_code ec;
	bool res = std::filesystem::remove(_target, ec);

	if (res)
	{
		std::wstring log_message = L"File " + _target.wstring() + L" has been deleted";
		logger.log(log_message);
	}
	else
	{
		std::wstring error_message = getLastError();
		logger.log(error_message);
	}
}
