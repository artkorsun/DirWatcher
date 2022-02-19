#pragma once

#include <functional>
#include <set>
#include <atomic>

class CommandManager
{
public:
	using CommandHandler = std::function<void()>;
	CommandManager(CommandHandler log_handler,
				   CommandHandler quit_handler,
				   const std::atomic<bool>& stop);

	void run() const;

private:
	void help() const;
	
	std::vector<std::tuple<std::set<std::wstring>, std::wstring, CommandHandler> > _commands;
	const std::atomic<bool>& _stop;
};