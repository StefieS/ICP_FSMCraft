/**
 * @file QTTransitionEnd.h
 * @brief Custom transition for FSM ending with STOP
 * 
 * @author xnovakf00
 * @date 08.05.2025
 */

#pragma once

#include <QAbstractTransition>
#include <QString>
#include <QEvent>
#include "../messages/Message.h"
#include "QTfsm.h"

/**
 * @class QTTransitionEnd
 * @brief Class to handle the transition on stop in the finite state machine (FSM).
 */
class QTTransitionEnd : public QAbstractTransition {
private:
    QTfsm* automaton; /**< Pointer to the FSM (automaton) associated with this transition. */

public:
    /**
     * @brief Constructor for the QTTransitionEnd class.
     * 
     * Initializes the transition and associates it with a parent state and FSM.
     * 
     * @param parentState The parent state in which this transition resides.
     * @param fsm Pointer to the FSM associated with this transition.
     */
    QTTransitionEnd(QState* parentState, QTfsm* fsm)
        : QAbstractTransition(parentState),
          automaton(fsm) {}

protected:
    /**
     * @brief Executes the transition action when triggered.
     * 
     * This method is invoked when the transition is triggered. It creates a stop message 
     * and sends it to the host through the FSM's `NetworkHandler`.
     * 
     * @param event The event that triggered the transition (not used in this case).
     */
    void onTransition(QEvent*) override {
        Message msg = Message(); // Create a Message object.
        msg.buildStopMessage(); // Build the stop message.
        automaton->getNetworkHandler().sendToHost(msg.toMessageString()); // Send the message to the host.
    }
};