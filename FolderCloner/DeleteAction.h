#pragma once

#include "Action.h"

#include <filesystem>

class DeleteAction : public Action
{
public:
	DeleteAction(std::filesystem::path target);

	void execute(Logger& logger) override;

private:
	std::filesystem::path _target;
};