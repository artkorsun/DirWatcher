#include "DeleteAction.h"
#include "Utils.h"

namespace fs = std::filesystem;

DeleteAction::DeleteAction(fs::path target)
	:	_target(std::move(target))
{
}

void DeleteAction::execute(Logger& logger)
{
	std::error_code ec;
	fs::remove_all(_target, ec);

	if (!ec) {
		logger.info(L"File " + _target.wstring() + L" has been deleted");
	}
	else {
		logger.error(L"Failed to delete " + _target.wstring() + L", " + getLastError());
	}
}
