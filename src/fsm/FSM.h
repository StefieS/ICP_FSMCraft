#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "State.h"
#include "Transition.h"
#include "InternalVar.h"

class FSM {
private:
    std::string name;
    std::string comment;
    std::map<std::string, std::shared_ptr<State>> states;
    std::vector<std::shared_ptr<Transition>> transitions;
    std::map<std::string, std::string> inputMemory; // added input memory
    std::vector<std::string> inputNames;
    std::vector<std::string> outputNames;
    std::string currentState;
    std::vector<InternalVar> internalVars;
    bool isActive;

public:
    FSM(const std::string& name);

    void addState(const std::shared_ptr<State>& state);
    void addTransition(const std::shared_ptr<Transition>& transition);
    void addInternalVar(const InternalVar& var);

    void setInitialState(const std::string& stateName);
    void setInput(const std::string& inputName, const std::string& value); // added
    void process(); // renamed from processEvent

    const std::string& getCurrentState() const;
    void printStatus() const; // added

    // getters
    std::string getName();
    std::map<std::string, std::shared_ptr<State>> getStates();
    std::vector<std::shared_ptr<Transition>> getTransitions();
    std::vector<std::string> getInputNames();
    std::vector<std::string> getOutputNames();
    std::vector<InternalVar> getInternalVars();

};
