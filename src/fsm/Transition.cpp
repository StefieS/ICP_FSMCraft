#include "Transition.h"

Transition::Transition(const std::string& src, const std::string& tgt, 
    const std::string& input, const std::string& cond)
: source(src), target(tgt), inputEvent(input), guardCondition(cond) {}


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

const std::string& Transition::getInputEvent() const {
    return inputEvent;
}

const std::string& Transition::getGuardCondition() const {
    return guardCondition;
}

const std::optional<int>& Transition::getDelay() const {
    return delayMs;
}
