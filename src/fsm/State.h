/**
 * @file State.h
 * @brief Declares the State class used in the FSM model.
 * @author xnovakf00
 * @author xmarina00
 * @date 07.05.2025
 */

#pragma once

#include <string>
#include <vector>
#include "Transition.h"

/**
 * @class State
 * @brief Represents a state in a finite state machine.
 * 
 * Each state can have an associated action (e.g., code to execute upon entry),
 * a set of transitions, and flags indicating whether it's an initial or final state.
 */
class State {
private:
    /**
     * Name of the state.
     */
    std::string name;

    /**
     * JavaScript action code executed when the state is entered.
     */
    std::string actionCode;

    /**
     * List of transitions originating from this state.
     */
    std::vector<Transition> transitions;

    /**
     * Indicates if this state is the initial state.
     */
    bool isInitial;

    /**
     * Indicates if this state is a final (terminal) state.
     */
    bool isFinal;

public:
    /**
     * @brief Constructs a new State object.
     * 
     * @param name Name of the state.
     * @param actionCode Code to execute when the state is entered.
     * @param isInitial Optional. True if this is the initial state.
     * @param isFinal Optional. True if this is the final state.
     */
    State(const std::string& name, const std::string& actionCode, bool isInitial = false, bool isFinal = false);

    /**
     * @brief Gets the name of the state.
     * @return A constant reference to the state's name.
     */
    const std::string& getName() const;

    /**
     * @brief Gets the action code associated with this state.
     * @return A constant reference to the action code.
     */
    const std::string& getActionCode() const;

    /**
     * @brief Gets the list of transitions from this state.
     * @return A constant reference to the list of transitions.
     */
    const std::vector<Transition>& getTransitions() const;

    /**
     * @brief Checks whether this state is marked as an initial state.
     * @return True if the state is initial; false otherwise.
     */
    bool isInitialState() const;

    /**
     * @brief Checks whether this state is marked as a final state.
     * @return True if the state is final; false otherwise.
     */
    bool isFinalState() const;

    /**
     * @brief Sets whether the state is an initial state.
     * @param val True to mark as initial; false otherwise.
     */
    void setInitial(bool val);

    /**
     * @brief Sets the name of the state.
     * @param newName New name to assign to the state.
     */
    void setName(const std::string& newName);

    /**
     * @brief Sets the action code for the state.
     * @param code Code to execute upon entering the state.
     */
    void setActionCode(const std::string& code);

    /**
     * @brief Adds a new transition to this state.
     * @param transition The transition to add.
     */
    void addTransition(const Transition& transition);

    /**
     * @brief Sets whether this state is a final state.
     * @param val True to mark as final; false otherwise.
     */
    void setFinal(bool val);
};
