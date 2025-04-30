#include "State.h"

State::State(const std::string& name, const std::string& actionCode, bool isInitial)
    : name(name), actionCode(actionCode), isInitial(isInitial) {}

const std::string& State::getName() const {
    return name;
}

const std::string& State::getActionCode() const {
    return actionCode;
}

bool State::isInitialState() const {
    return isInitial;
}
