#include "ActionQueue.h"

#include <algorithm>
#include <iterator>


void ActionQueue::add(std::vector<std::shared_ptr<Action>> actions)
{
	std::lock_guard g(_mutex);
	std::move(std::begin(actions), std::end(actions), std::back_inserter(_queue));
}

void ActionQueue::add_deferred(std::multimap<std::time_t, std::shared_ptr<Action>> deferred_actions)
{
	std::lock_guard g(_mutex);
	_deferred_actions.merge(deferred_actions);
}

std::vector<std::shared_ptr<Action>> ActionQueue::retrieve()
{
	std::lock_guard g(_mutex);

	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	const auto deferred_actions_to_execute_itr = _deferred_actions.upper_bound(now);

	std::for_each(std::begin(_deferred_actions), deferred_actions_to_execute_itr, [&](auto& item) {
		_queue.emplace_back(std::move(item.second));
		});
	_deferred_actions.erase(std::begin(_deferred_actions), deferred_actions_to_execute_itr);

	std::vector<std::shared_ptr<Action>> res;
	_queue.swap(res);

	return res;
}
