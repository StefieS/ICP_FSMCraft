/**
 * @file FSM.cpp
 * @brief Implementation file for the common representation of the FSM
 * @author xnovakf00
 * @author xmarina00
 * @date 07.05.2025
 */


#include "FSM.h"
#include "InternalVar.h"
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

void FSM::addInternalVar(InternalVar var) {
    internalVars.push_back(var);
}

void FSM::addInputName(const std::string& inputName) {
    inputNames.push_back(inputName);
}

void FSM::addOutputName(const std::string& outputName) {
    outputNames.push_back(outputName);
}

std::string FSM::getName() {
    return name;
}

std::map<std::string, std::shared_ptr<State>> FSM::getStates() {
    return states;
}

std::vector<std::shared_ptr<Transition>> FSM::getTransitions() {
    return transitions;
}

std::vector<std::string> FSM::getInputNames() {
    return inputNames;
}

std::vector<std::string> FSM::getOutputNames() {
    return outputNames;
}

std::vector<InternalVar> FSM::getInternalVars() {
    return internalVars;
}   

