#pragma once
#include <string>

class State {
private:
    std::string name;
    std::string actionCode;
    // TODO: Transition state arr
    bool isActive;
    bool isInitial;
    bool isFinal;

public:
    State(const std::string& name, const std::string& actionCode, bool isInitial = false);
    
    const std::string& getName() const;
    const std::string& getActionCode() const;
    bool isInitialState() const;

    void setActionCode(const std::string& code);
};
