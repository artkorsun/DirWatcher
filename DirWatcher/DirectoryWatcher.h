#pragma once

#include "ActionQueue.h"
#include "Logger.h"

#include <deque>
#include <string>
#include <memory>
#include <atomic>

#include <windows.h>

class DirectoryWatcher
{
public:
	DirectoryWatcher(const std::wstring& source, 
					 const std::wstring& target,
					 ActionQueue& action_queue,
					 const std::atomic<bool>& stop,
					 Logger& logger);

	bool run(HANDLE io_completion_port);

private:
	std::wstring _source;
	std::wstring _target;
	ActionQueue& _action_queue;
	const std::atomic<bool>& _stop;
	Logger& _logger;
};