#pragma once

#include "Action.h"

#include <vector>
#include <map>
#include <mutex>
#include <memory>

class ActionQueue
{
public:
	void add(std::vector<std::shared_ptr<Action>> actions);
	void add_deferred(std::multimap<std::time_t, std::shared_ptr<Action>> deferred_actions);
	std::vector<std::shared_ptr<Action>> retrieve();

private:
	std::vector<std::shared_ptr<Action>> _queue;
	std::multimap<std::time_t, std::shared_ptr<Action>> _deferred_actions;
	std::mutex _mutex;
};