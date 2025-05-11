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
     * necessary? TODO
     */
    std::map<std::string, std::string> inputMemory;
    /**
     * Vector of all defined input names
     */
    std::vector<std::string> inputNames;
    /**
     * Vector of all defined output names
     */
    std::vector<std::string> outputNames;
    /**
     * todo necessary?
     */
    std::string currentState;
    /**
     * Vector of all internal vars
     */
    std::vector<InternalVar> internalVars;
    /**
     * todo necessary
     */
    bool isActive;

public:
    FSM(const std::string& name);

    void addState(const std::shared_ptr<State>& state);
    void addTransition(const std::shared_ptr<Transition>& transition);
    void addInternalVar(InternalVar var);
    void addInputName(const std::string& inputName);
    void addOutputName(const std::string& outputName);
    void setInitialState(const std::string& stateName);
    void setInput(const std::string& inputName, const std::string& value);
    void process(); // renamed from processEvent

    const std::string& getCurrentState() const;
    /**
     * @todo necessary 
     */
    void printStatus() const;

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
