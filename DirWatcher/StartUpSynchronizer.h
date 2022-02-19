#pragma once

#include "ActionQueue.h"
#include "Logger.h"

#include <string>

class StartUpSynchronizer
{
public:
	StartUpSynchronizer(const std::wstring& source, 
						const std::wstring& target, 
						ActionQueue& action_queue,
						Logger& logger);
	void perform();

private:
	std::wstring _source;
	std::wstring _target;
	ActionQueue& _action_queue;
	Logger& _logger;
};