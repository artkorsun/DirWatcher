#pragma once

#include "Action.h"

#include <vector>
#include <mutex>
#include <memory>

class ActionQueue
{
public:
	void add(std::vector<std::shared_ptr<Action>> actions);
	std::vector<std::shared_ptr<Action>> retrieve();

private:
	std::vector<std::shared_ptr<Action>> _queue;
	std::mutex _mutex;
};