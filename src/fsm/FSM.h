#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "State.h"
#include "Transition.h"

class FSM {
private:
    std::string name;
    std::string comment;
    std::map<std::string, std::shared_ptr<State>> states;
    std::vector<std::shared_ptr<Transition>> transitions;
    std::map<std::string, std::string> inputMemory; // added input memory
    std::string currentState;
    bool isActive;

public:
    FSM(const std::string& name);

    void addState(const std::shared_ptr<State>& state);
    void addTransition(const std::shared_ptr<Transition>& transition);

    void setInitialState(const std::string& stateName);
    void setInput(const std::string& inputName, const std::string& value); // added
    void process(); // renamed from processEvent

    const std::string& getCurrentState() const;
    void printStatus() const; // added
};
