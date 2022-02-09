#pragma once

#include "Logger.h"

class Action
{
public:
	virtual void execute(Logger& logger) = 0;
};