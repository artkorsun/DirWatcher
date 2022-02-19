#include "CommandManager.h"

#include <iostream>

CommandManager::CommandManager(CommandHandler log_handler,
							   CommandHandler quit_handler,
							   const std::atomic<bool>& stop)
	:	_stop(stop)
{
	_commands.push_back({ { L"l", L"log" }, L"Show log information", log_handler});
	_commands.push_back({ { L"q", L"quit" }, L"Quit the application", quit_handler});
	_commands.push_back({ { L"h", L"help" }, L"Show help", std::bind(&CommandManager::help, this)});
}

void CommandManager::run() const
{
	std::wcout << "Enter a command or \"help\" for more information\n";

	while (!_stop.load()) {
		std::wcout << "\n>";
		std::wstring input;
		std::wcin >> input;

		bool found = false;
		for (const auto& commands_desc : _commands) {
			if (const auto cmd_itr = std::get<0>(commands_desc).find(input); cmd_itr != std::get<0>(commands_desc).end()) {
				found = true;
				const auto& handler = std::get<2>(commands_desc);
				handler();
			}
		}

		if (!found) {
			std::wcout << "\nUnexpected command\n";
		}
	}
}

void CommandManager::help() const 
{
	std::wcout << L"DirMimicker watches changes in a source directory and performs the same actions to fo files in a target directory\n";
	for (const auto& command_desc : _commands) {
		std::wstring cmds;
		for (const auto& cmd : std::get<0>(command_desc)) {
			if (!cmds.empty()) {
				cmds += L", ";
			}
			cmds += cmd;
		}

		std::wcout << cmds << "\t-\t" << std::get<1>(command_desc) << std::endl;
	}
}
