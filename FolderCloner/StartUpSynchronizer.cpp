#include "StartUpSynchronizer.h"
#include "CopyAction.h"
#include "DeleteAction.h"

#include <filesystem>
#include <iostream>

StartUpSynchronizer::StartUpSynchronizer(const std::wstring& source, 
										 const std::wstring& target, 
										 ActionQueue& action_queue)
	:	_source(source),
		_target(target),
		_action_queue(action_queue)
{
}

void StartUpSynchronizer::perform()
{
	namespace fs = std::filesystem;
	
	std::vector<std::shared_ptr<Action>> actions;

	for (auto const& source_dir_entry : fs::recursive_directory_iterator(_source))
	{
		auto target_file = fs::path{ _target } / fs::relative(source_dir_entry, _source);

		if (!fs::exists(target_file)) {
			actions.emplace_back(std::make_shared<CopyAction>(fs::absolute(source_dir_entry).wstring(),	fs::absolute(target_file).wstring()));
		}
	}

	for (auto const& target_dir_entry : fs::recursive_directory_iterator(_target))
	{
		auto source_file = fs::path{ _target } / fs::relative(target_dir_entry, _source);

		if (!fs::exists(source_file)) {
			actions.emplace_back(std::make_shared<DeleteAction>(fs::absolute(target_dir_entry).wstring()));
		}
	}

	if (!actions.empty()) {
		_action_queue.add(actions);
	}
}