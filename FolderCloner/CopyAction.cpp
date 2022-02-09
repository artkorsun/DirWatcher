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
	const auto co = fs::copy_options::overwrite_existing | fs::copy_options::recursive;
		
	std::error_code ec;
	fs::copy(_source, _target, co, ec);

	if (!ec)
	{
		std::wstring log_message = L"File " + _source.wstring() + L" has been copied to " + _target.wstring();
		logger.log(log_message);
	}
	else
	{
		std::wstring error_message = getLastError();
		logger.log(error_message);
	}
}
