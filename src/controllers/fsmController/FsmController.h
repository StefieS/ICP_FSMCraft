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
class FsmController {
    private:
    QTfsm *qtfsm;

    public:
    FsmController();
    const Message& performAction(Message &msg);
};