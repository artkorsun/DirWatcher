#pragma once

#include "Action.h"

#include <filesystem>

class RenameAction : public Action
{
public:
	RenameAction(std::filesystem::path source, std::filesystem::path target);

	void execute(Logger& logger) override;

private:
	std::filesystem::path _source;
	std::filesystem::path _target;
};