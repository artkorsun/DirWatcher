#pragma once

#include "Action.h"

#include <windows.h>
#include <filesystem>

class CopyAction : public Action
{
public:
	CopyAction(std::filesystem::path source, std::filesystem::path target);

	void execute(Logger& logger) override;

private:
	std::filesystem::path _source;
	std::filesystem::path _target;
};