/**
 * @file Transition.h
 * @brief Declares the Transition class used in FSM modeling.
 * @author xnovakf00
 * @author xmarina00
 * @date 07.05.2025
 */

#pragma once

#include <string>
#include <optional>

/**
 * @class Transition
 * @brief Represents a state transition in a finite state machine.
 */
class Transition {
private:
    /** 
     * @brief Name of the source state.
     */
    std::string source;

    /** 
     * @brief Name of the target state.
     */
    std::string target;

    /** 
     * @brief Input event that triggers the transition.
     */
    std::string inputEvent;

    /** 
     * @brief Condition that must be true for the transition to occur.
     */
    std::string guardCondition;

    /** 
     * @brief Optional delay (in milliseconds) before the transition occurs.
     */
    std::string delayMs;

public:
    /**
     * @brief Constructs a new Transition object.
     * 
     * @param src Name of the source state.
     * @param tgt Name of the target state.
     * @param input Input event triggering the transition.
     * @param cond Guard condition for the transition.
     * @param timeout Delay before the transition, in milliseconds.
     */
    Transition(const std::string& src, 
               const std::string& tgt, 
               const std::string& input, 
               const std::string& cond, 
               const std::string& timeout);

    /**
     * @brief Sets the input event.
     * @param input Input event name.
     */
    void setInputEvent(const std::string& input);

    /**
     * @brief Sets the guard condition.
     * @param cond A logical expression used as the guard condition.
     */
    void setGuardCondition(const std::string& cond);

    /**
     * @brief Sets the delay duration.
     * @param ms Delay in milliseconds as a string.
     */
    void setDelay(std::string ms);

    /**
     * @brief Gets the source state name.
     * @return A constant reference to the source state name.
     */
    const std::string& getSource() const;

    /**
     * @brief Gets the target state name.
     * @return A constant reference to the target state name.
     */
    const std::string& getTarget() const;

    /**
     * @brief Gets the input event name.
     * @return A constant reference to the input event name.
     */
    const std::string& getInputEvent() const;

    /**
     * @brief Gets the guard condition.
     * @return A constant reference to the guard condition string.
     */
    const std::string& getGuardCondition() const;

    /**
     * @brief Gets the delay value in milliseconds.
     * @return A constant reference to the delay string.
     */
    const std::string& getDelay() const;
};
