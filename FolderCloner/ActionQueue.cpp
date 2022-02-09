#include "ActionQueue.h"

#include <iterator>

void ActionQueue::add(std::vector<std::shared_ptr<Action>> actions)
{
	std::lock_guard g(_mutex);
	std::move(std::begin(actions), std::end(actions), std::back_inserter(_queue));
}

std::vector<std::shared_ptr<Action>> ActionQueue::retrieve()
{
	std::lock_guard g(_mutex);
	std::vector<std::shared_ptr<Action>> res;
	_queue.swap(res);

	return res;
}
