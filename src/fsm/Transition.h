#pragma once
#include <string>
#include <optional>

class Transition {
private:
    std::string source;
    std::string target;
    std::string inputEvent;
    std::string guardCondition;
    std::optional<int> delayMs;
    bool isActive;

public:
    Transition(const std::string& src, const std::string& tgt, const std::string& input, const std::string& cond);
    void setInputEvent(const std::string& input);
    void setGuardCondition(const std::string& cond);
    void setDelay(int ms);

    const std::string& getSource() const;
    const std::string& getTarget() const;
    const std::string& getInputEvent() const;
    const std::string& getGuardCondition() const;
    const std::optional<int>& getDelay() const;
};
