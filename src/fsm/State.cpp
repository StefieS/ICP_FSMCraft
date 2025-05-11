/**
 * @file State.cpp
 * @brief Implements the State class used in the FSM model.
 * @author xnovakf00
 * @author xmarina00
 * @date 07.05.2025
 */

#include "State.h"

State::State(const std::string& name, const std::string& actionCode, bool isInitial, bool isFinal)
    : name(name), actionCode(actionCode), transitions(), isInitial(isInitial), isFinal(isFinal) {}


const std::string& State::getName() const {
    return name;
}

const std::string& State::getActionCode() const {
    return actionCode;
}

bool State::isInitialState() const {
    return isInitial;
}

bool State::isFinalState() const {
    return isFinal;
}

void State::addTransition(const Transition& transition) {
    transitions.push_back(transition);
}

void State::setFinal(bool val) {
    isFinal = val;
}

const std::vector<Transition>& State::getTransitions() const {
    return transitions;
}

void State::setActionCode(const std::string& code) {
    actionCode = code;
}

void State::setInitial(bool val) {
    isInitial = val;
}

void State::setName(const std::string& newName) {
    name = newName;
}
