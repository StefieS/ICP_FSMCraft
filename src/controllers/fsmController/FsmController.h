/**
 * @file FsmController.h
 * @brief Header file for the FsmController class
 * @author xnovakf00
 * @author xlesigm00
 * @date 09.05.2025
 */


#pragma once
#include "../../qtfsm/QTfsm.h"
#include "../../messages/Message.h"
#include "../../qtfsm/QTfsmBuilder.h"
#include <QFile>

/**
 * @class FsmController
 * @brief Controls the inner FSM based on the received message from
 * client gui
 */
class FsmController {
    private:
    /**
     * Fsm to controls
     */
    QTfsm *qtfsm;

    public:
    /**
     * @brief Constructor
     */
    FsmController();
    /**
     * @brief Performs actions on the FSM based on message
     * @param msg Message to base the action on
     * @return Message for further sending
     */
    const Message performAction(Message &msg);
    /**
     * @brief Gets the fsm it is controlling
     * @return Fsm controlling
     */
    QTfsm* getFsm();
};