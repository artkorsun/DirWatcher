#include "CopyAction.h"
#include "Utils.h"

namespace fs = std::filesystem;

CopyAction::CopyAction(fs::path source, fs::path target)
	:	_source(std::move(source)), 
		_target(std::move(target))
{
}

void CopyAction::execute(Logger& logger)
{
	const auto co = fs::copy_options::overwrite_existing;
		
	std::error_code ec;
	fs::copy(_source, _target, co, ec);

	if (!ec) {
		logger.info(L"File " + _source.wstring() + L" has been copied to " + _target.wstring());
	}
	else {
		logger.error(L"Failed to copy " + _source.wstring() + L" to " + _target.wstring() + L", " + getLastError());
	}
}
