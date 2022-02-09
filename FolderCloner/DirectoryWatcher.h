#pragma once

#include "ActionQueue.h"

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
					 const std::atomic<bool>& stop);

	void run(HANDLE& watch_event);

private:
	std::wstring _source;
	std::wstring _target;
	ActionQueue& _action_queue;
	const std::atomic<bool>& _stop;
};