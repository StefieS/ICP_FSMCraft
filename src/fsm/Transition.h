#pragma once
#include <string>
#include <optional>

class Transition {
private:
    std::string source;
    std::string target;
    std::optional<std::string> inputEvent;
    std::optional<std::string> guardCondition;
    std::optional<int> delayMs;
    bool isActive;

public:
    Transition(const std::string& src, const std::string& tgt);

    void setInputEvent(const std::string& input);
    void setGuardCondition(const std::string& cond);
    void setDelay(int ms);

    const std::string& getSource() const;
    const std::string& getTarget() const;
    const std::optional<std::string>& getInputEvent() const;
    const std::optional<std::string>& getGuardCondition() const;
    const std::optional<int>& getDelay() const;
};
