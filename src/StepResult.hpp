#pragma once
#include <string>
enum class StepStatus {
    EXECUTED,
    HALTED,
    INVALID_INSTRUCTION,
    FETCH_ERROR,
    STEP_LIMIT
};
struct StepResult {
    StepStatus status =StepStatus::EXECUTED;
    std::string message;
};
