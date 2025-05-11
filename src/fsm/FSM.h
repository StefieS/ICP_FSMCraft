/**
 * @file FSM.h
 * @brief Header file for the common representation of the FSM
 * @author xnovakf00
 * @author xmarina00
 * @date 07.05.2025
 */

#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "State.h"
#include "Transition.h"
#include "InternalVar.h"

/**
 * @class FSM
 * @brief Holds necessary information for seamless transformation
 * between GUI-fsm and FSM performing logic.
 */
class FSM {
private:
    /**
     * Name of the fsm
     */
    std::string name;

    /**
     * Description of the FSM
     */
    std::string comment;

    /**
     * A map of states based on their name
     */
    std::map<std::string, std::shared_ptr<State>> states;

    /**
     * A vector of transitions
     */
    std::vector<std::shared_ptr<Transition>> transitions;

    /**
     * Vector of all defined input names
     */
    std::vector<std::string> inputNames;

    /**
     * Vector of all defined output names
     */
    std::vector<std::string> outputNames;

    /** 
     * Name of the current state 
     */
    std::string currentState;

    /**
     * Vector of all internal vars
     */
    std::vector<InternalVar> internalVars;

public:
    /**
     * @brief Constructs a new FSM object with a given name.
     * @param name The name of the FSM.
     */
    FSM(const std::string& name);

    /**
     * @brief Adds a new state to the FSM.
     * @param state A shared pointer to the State to add.
     */
    void addState(const std::shared_ptr<State>& state);

    /**
     * @brief Adds a new transition to the FSM.
     * @param transition A shared pointer to the Transition to add.
     */
    void addTransition(const std::shared_ptr<Transition>& transition);

    /**
     * @brief Adds an internal variable to the FSM.
     * @param var The internal variable to add.
     */
    void addInternalVar(InternalVar var);

    /**
     * @brief Adds a new input variable name.
     * @param inputName The name of the input variable.
     */
    void addInputName(const std::string& inputName);

    /**
     * @brief Adds a new output variable name.
     * @param outputName The name of the output variable.
     */
    void addOutputName(const std::string& outputName);

    /**
     * @brief Sets the initial state of the FSM.
     * @param stateName The name of the state to set as initial.
     */
    void setInitialState(const std::string& stateName);

    /**
     * @brief Retrieves the name of the current state.
     * @return A const reference to the current state's name.
     */
    const std::string& getCurrentState() const;

    /**
     * @brief Gets the name of the finite state machine (FSM).
     * @return A std::string representing the name of the FSM.
     */
    std::string getName();

    /**
     * @brief Retrieves all states defined in the FSM.
     * @return A std::map where keys are state names and values are shared pointers to State objects.
     */
    std::map<std::string, std::shared_ptr<State>> getStates();

    /**
     * @brief Retrieves all transitions defined in the FSM.
     * @return A std::vector containing shared pointers to Transition objects.
     */
    std::vector<std::shared_ptr<Transition>> getTransitions();

    /**
     * @brief Gets the list of input variable names used by the FSM.
     * @return A std::vector of input variable names as std::strings.
     */
    std::vector<std::string> getInputNames();

    /**
     * @brief Gets the list of output variable names used by the FSM.
     * @return A std::vector of output variable names as std::strings.
     */
    std::vector<std::string> getOutputNames();

    /**
     * @brief Retrieves all internal variables used by the FSM.
     * @return A std::vector of InternalVar objects representing internal state.
     */
    std::vector<InternalVar> getInternalVars();
};
