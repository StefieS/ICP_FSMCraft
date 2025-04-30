#include "FSM.h"
#include <iostream>

FSM::FSM(const std::string& name)
    : name(name), currentState("") {}

void FSM::addState(const std::shared_ptr<State>& state) {
    states[state->getName()] = state;
    if (state->isInitialState()) {
        currentState = state->getName();
    }
}

void FSM::addTransition(const std::shared_ptr<Transition>& transition) {
    transitions.push_back(transition);
}

void FSM::setInitialState(const std::string& stateName) {
    if (states.count(stateName)) {
        currentState = stateName;
    }
}

void FSM::setInput(const std::string& inputName, const std::string& value) {
    inputMemory[inputName] = value;
}

const std::string& FSM::getCurrentState() const {
    return currentState;
}

void FSM::printStatus() const {
    std::cout << "Current state: " << currentState << std::endl;
    std::cout << "Inputs: ";
    for (const auto& [key, val] : inputMemory) {
        std::cout << key << "=" << val << " ";
    }
    std::cout << std::endl;
}

void FSM::process() {
    std::cout << "Processing event in state: " << currentState << std::endl;

    for (const auto& transition : transitions) {
        if (transition->getSource() != currentState) continue;

        auto input = transition->getInputEvent();
        if (input.has_value()) {
            auto it = inputMemory.find(input.value());
            if (it == inputMemory.end()) continue;

            std::cout << "Transitioning from " << transition->getSource()
                      << " to " << transition->getTarget()
                      << " on input: " << input.value() << std::endl;

            currentState = transition->getTarget();
            break;
        }
    }

    auto it = states.find(currentState);
    if (it != states.end()) {
        std::cout << "Action code: " << it->second->getActionCode() << std::endl;
    }
}
