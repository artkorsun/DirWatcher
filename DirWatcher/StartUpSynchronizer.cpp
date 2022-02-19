#include "StartUpSynchronizer.h"
#include "CopyAction.h"
#include "DeleteAction.h"
#include "Utils.h"

#include <filesystem>
#include <sstream>
#include <iostream>

StartUpSynchronizer::StartUpSynchronizer(const std::wstring& source, 
										 const std::wstring& target, 
										 ActionQueue& action_queue,
										 Logger& logger)
	:	_source(source),
		_target(target),
		_action_queue(action_queue),
		_logger(logger)
{
}

void StartUpSynchronizer::perform()
{
	namespace fs = std::filesystem;
	
	std::vector<std::shared_ptr<Action>> actions;

	// Copy files which don't exist in target folder
	for (auto const& source_dir_entry : fs::directory_iterator(_source))
	{
		if (source_dir_entry.is_directory()) {
			continue;
		}

		try {
			auto original_ext = source_dir_entry.path().extension();
			auto filename = fs::relative(source_dir_entry, _source).replace_extension(original_ext.wstring() + L".bak");
			auto target_file = fs::path{ _target } / filename;

			if (!fs::exists(target_file)) {
				actions.emplace_back(std::make_shared<CopyAction>(fs::absolute(source_dir_entry).wstring(), fs::absolute(target_file).wstring()));
			}
		}
		catch (const std::filesystem::filesystem_error&) {
			_logger.error(L"Error during start up synchronization, " + getLastError());
		}
	}

	// Remove files from target folder if they don't exist in source
	for (auto const& target_dir_entry : fs::recursive_directory_iterator(_target))
	{
		try {
			// Remove all non '.bak' files
			if (target_dir_entry.path().extension().wstring() != L".bak") {
				actions.emplace_back(std::make_shared<DeleteAction>(fs::absolute(target_dir_entry).wstring()));
			}
			else {
				auto rel = fs::relative(target_dir_entry, _target);
				auto source_file = fs::path{ _source } / rel.stem();

				if (!fs::exists(source_file)) {
					actions.emplace_back(std::make_shared<DeleteAction>(fs::absolute(target_dir_entry).wstring()));
				}
			}
		}
		catch (const std::filesystem::filesystem_error&) {
			_logger.error(L"Error during start up synchronization, " + getLastError());
		}
	}

	if (!actions.empty()) {
		_action_queue.add(actions);
	}
}