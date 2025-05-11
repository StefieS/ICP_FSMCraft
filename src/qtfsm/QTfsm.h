/**
 * @file QTfsm.h
 * @brief Header of the QTfsm class representing inner logic and transitions of the FSM defined.
 * 
 * @author xlesigm00
 * @author xnovakf00
 * @date 05.05.2025
 */

#pragma once
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QObject>
#include <QJSEngine>
#include "../networkHandler/NetworkHandler.h"
#include "QTBuiltinHandler.h"

class QTBuiltinHandler; // Forward declaration for QTBuiltinHandler

/**
 * @class QTfsm
 * @brief Class representing a Finite State Machine (FSM) in Qt.
 */
class QTfsm : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor for the QTfsm class.
     * 
     * Initializes the FSM with optional parameters for parent object and FSM name.
     * 
     * @param parent The parent QObject, defaults to nullptr.
     * @param name The name of the FSM, defaults to an empty string.
     */
    QTfsm(QObject* parent = nullptr, const std::string& name = "");

    /**
     * @brief Starts the state machine.
     */
    void start();

    /**
     * @brief Stops the state machine and emits a stop signal.
     */
    void stop();

    /**
     * @brief Emits a stop signal to indicate the machine should stop.
     */
    void emitStopSignal();

    // Setup methods for FSM
    /**
     * @brief Adds a state to the state machine.
     * 
     * @param name The name of the state.
     * @return Pointer to the created QState.
     */
    QState* addState(const QString& name);

    /**
     * @brief Sets the initial state of the state machine.
     * 
     * @param state Pointer to the state to set as initial.
     */
    void setInitialState(QAbstractState* state);

    /**
     * @brief Adds a final state to the state machine.
     * 
     * @param name The name of the final state.
     * @return Pointer to the created QFinalState.
     */
    QFinalState* addFinalState(const QString& name);

    /**
     * @brief Adds a JavaScript action to a state.
     * 
     * @param state The state to which the action will be added.
     * @param jsCode The JavaScript code to execute during the state's transition.
     */
    void addStateJsAction(QState* state, const QString& jsCode);

    /**
     * @brief Adds a JavaScript-based transition between two states.
     * 
     * @param from The initial state of the transition.
     * @param to The target state of the transition.
     * @param jsCondition The JavaScript condition for the transition.
     * @param expectedInput The expected input for the transition.
     * @param timeout The timeout condition for the transition.
     * @param id Id of the transition for gui later.
     */
    void addJsTransition(QState* from, 
                         QAbstractState* to, 
                         const QString& jsCondition, 
                         const QString& expectedInput, 
                         const QString& timeout,
                         int id);

    /**
     * @brief Sets a JavaScript variable in the engine.
     * 
     * @param name The name of the variable.
     * @param value The value of the variable.
     */
    void setJsVariable(const QString& name, const QJSValue& value);

    /**
     * @brief Sets an output value for the FSM.
     * 
     * @param name The name of the output.
     * @param value The value of the output.
     */
    void setOutput(const QString& name, const QJSValue& value);

    /**
     * @brief Sets an input value for the FSM.
     * 
     * @param name The name of the input.
     * @param value The value of the input.
     */
    void setInput(const QString& name, const QJSValue& value);

    /**
     * @brief Initializes the JavaScript engine for the FSM.
     */
    void initializeJsEngine();

    // Getter methods
    /**
     * @brief Retrieves the name of the FSM.
     * 
     * @return The name of the FSM.
     */
    std::string getName();

    /**
     * @brief Retrieves the JavaScript engine for the FSM.
     * 
     * @return Pointer to the QJSEngine used in the FSM.
     */
    QJSEngine* getJsEngine();

    /**
     * @brief Retrieves the state machine.
     * 
     * @return Pointer to the QStateMachine.
     */
    QStateMachine* getMachine();

    /**
     * @brief Retrieves a state by its name.
     * 
     * @param name The name of the state.
     * @return Pointer to the state, or nullptr if not found.
     */
    QAbstractState* getStateByName(const QString& name) const;

    /**
     * @brief Retrieves the NetworkHandler associated with this FSM.
     * 
     * @return Reference to the NetworkHandler.
     */
    NetworkHandler& getNetworkHandler();

    /**
     * @brief Retrieves all input values of the FSM.
     * 
     * @return A map of input names and their values.
     */
    std::map<std::string, QJSValue> getInputs();

    /**
     * @brief Retrieves all output values of the FSM.
     * 
     * @return A map of output names and their values.
     */
    std::map<std::string, QJSValue> getOutputs();

    /**
     * @brief Retrieves all internal variables of the FSM.
     * 
     * @return A map of variable names and their values.
     */
    std::map<std::string, QJSValue> getVars();

    /**
     * @brief Converts a map of QJSValue to a map of strings.
     * 
     * @param map The map to convert.
     * @return A map with string keys and string values.
     */
    std::map<std::string, std::string> getStringMap(std::map<std::string, QJSValue> map);

    /**
     * @brief Posts an event to the event loop.
     * 
     * @param event Pointer to the event to post.
     */
    void postEvent(QEvent* event);

    QVariantMap map; /**< A map storing additional data related to the FSM for JSengine. */

signals:
    /**
     * @brief Signal emitted when the FSM is stopped.
     */
    void stopSignal();

private:
    std::string jsonName; /**< Name of the FSM in JSON format. */
    bool connected; /**< Connection status of the FSM. */
    QStateMachine machine; /**< The state machine for the FSM. */
    QState* automaton; /**< The main state of the FSM. */
    QFinalState* end; /**< The final state of the FSM. */
    QJSEngine engine; /**< The JavaScript engine for the FSM. */
    NetworkHandler networkHandler; /**< Network handler for communication. */
    std::map<std::string, QJSValue> outputValues; /**< Map of output values. */
    std::map<std::string, QJSValue> internalValues; /**< Map of internal variables. */
    std::map<std::string, QJSValue> inputValues; /**< Map of input values. */
    QTBuiltinHandler* builtinHandler; /**< Built-in handler for specific FSM actions. */
};
