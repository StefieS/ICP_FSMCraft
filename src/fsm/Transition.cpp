#include "Transition.h"

Transition::Transition(const std::string& src, const std::string& tgt)
    : source(src), target(tgt) {}

void Transition::setInputEvent(const std::string& input) {
    inputEvent = input;
}

void Transition::setGuardCondition(const std::string& cond) {
    guardCondition = cond;
}

void Transition::setDelay(int ms) {
    delayMs = ms;
}

const std::string& Transition::getSource() const {
    return source;
}

const std::string& Transition::getTarget() const {
    return target;
}

const std::optional<std::string>& Transition::getInputEvent() const {
    return inputEvent;
}

const std::optional<std::string>& Transition::getGuardCondition() const {
    return guardCondition;
}

const std::optional<int>& Transition::getDelay() const {
    return delayMs;
}
