#pragma once
#include <string>
#include "Transition.h"
#include <vector>

class State {
private:
    std::string name;
    std::string actionCode;
    std::vector<Transition> transitions;
    bool isActive;
    bool isInitial;
    bool isFinal;
    

public:
    State(const std::string& name, const std::string& actionCode, bool isInitial = false, bool isFinal = false);
    
    const std::string& getName() const;
    const std::string& getActionCode() const;
    const std::vector<Transition>& getTransitions() const;
    bool isInitialState() const;
    bool isFinalState() const;
    void setInitial(bool val);
    void setName(const std::string& newName);


    void setActionCode(const std::string& code);
    void addTransition(const Transition& transition);
    void setFinal(bool val);

};
