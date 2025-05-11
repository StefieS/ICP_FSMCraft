/**
 * @file QTBuiltinHandler.h
 * @brief Header file of built-in functions for the JS engine integration in QTfsm.
 * 
 * This class provides slots that can be used as built-in handlers within a QT-based
 * FSM system. It offers functionality such as checking if an output is defined,
 * measuring elapsed time, and tracking state transitions.
 * 
 * @author xnovakf00
 * @date 08.05.2025
 */

#pragma once

#include <QObject>
#include <QString>
#include "QTfsm.h"
#include <QElapsedTimer>

class QTfsm; ///< Forward declaration of QTfsm

/**
 * @class QTBuiltinHandler
 * @brief Provides built-in slots for use in the JS-based FSM scripting environment.
 *
 * The QTBuiltinHandler class exposes several common functions—such as output control,
 * time measurement, and state transition handling—that can be invoked from a JS context
 * inside the FSM system.
 */
class QTBuiltinHandler : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a QTBuiltinHandler object.
     * @param parent The parent QObject.
     * @param fsm Pointer to the FSM instance this handler operates on.
     */
    explicit QTBuiltinHandler(QObject* parent = nullptr, QTfsm* fsm = nullptr);

public slots:
    /**
     * @brief Sets the output
     * @param name Name of the output.
     * @param value Value to emit.
     */
    void output(const QString& name, const QString& value);

    /**
     * @brief Returns the number of milliseconds elapsed since the last state transition.
     * @return Time elapsed in milliseconds.
     */
    int elapsed();

    /**
     * @brief Checks whether the given output is defined in the JSengine.
     * @param name Name of the output.
     * @return True if defined, false otherwise.
     */
    bool defined(const QString& name);

    /**
     * @brief Updates the internal state tracking when a new state is entered.
     * @param newState The new state that was entered.
     */
    void stateEntered(QState* newState);

private:
    /** @brief Pointer to the FSM instance used by this handler. */
    QTfsm* fsm;

    /** @brief Timer used to track elapsed time since the last state transition. */
    QElapsedTimer stateTimer;

    /** 
     * @brief Last active state entered in the FSM, if states are same after transition
     * the time should not be reset
     */
    QState* lastActiveState = nullptr;
};
