#pragma once

#include "ActionQueue.h"

#include <string>

class StartUpSynchronizer
{
public:
	StartUpSynchronizer(const std::wstring& source, const std::wstring& target, ActionQueue& action_queue);
	void perform();

private:
	std::wstring _source;
	std::wstring _target;
	ActionQueue& _action_queue;
};