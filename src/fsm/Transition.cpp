/**
 * @file Transition.cpp
 * @brief Implements the Transition class used in FSM modeling.
 * @author xnovakf00
 * @date 07.05.2025
 */

#include "Transition.h"

Transition::Transition(const std::string& src,
    const std::string& tgt, 
    const std::string& input, 
    const std::string& cond,
    const std::string& timeout)
: source(src), target(tgt), inputEvent(input), guardCondition(cond), delayMs(timeout) {}


void Transition::setInputEvent(const std::string& input) {
    inputEvent = input;
}

void Transition::setGuardCondition(const std::string& cond) {
    guardCondition = cond;
}

void Transition::setDelay(std::string ms) {
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

const std::string& Transition::getDelay() const {
    return delayMs;
}