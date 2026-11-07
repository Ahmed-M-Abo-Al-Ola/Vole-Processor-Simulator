#pragma once
#include <string>

// scoped enums prevent unwanted implicit conversions (to int)
enum class StepStatus {
	EXECUTED,
	HALTED,
	INVALID_INSTRUCTION,
	FETCH_ERROR,
	STEP_LIMIT
};

struct StepResult {
	StepStatus status;
	std::string message;
};