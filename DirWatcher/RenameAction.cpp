#include "RenameAction.h"
#include "Utils.h"

namespace fs = std::filesystem;

RenameAction::RenameAction(fs::path source, fs::path target)
	:	_source(std::move(source)), 
		_target(std::move(target))
{
}

void RenameAction::execute(Logger& logger)
{
	std::error_code ec;
	fs::rename(_source, _target, ec);

	if (!ec) {
		logger.info(L"File " + _source.wstring() + L" has been renamed to " + _target.wstring());
	}
	else {
		logger.error(L"Failed to rename " + _source.wstring() + L" to " + _target.wstring() + L", " + getLastError());
	}
}
