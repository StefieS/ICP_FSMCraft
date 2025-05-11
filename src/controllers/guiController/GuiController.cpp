/**
 * @file GuiController.cpp
 * @brief Implementation for the GuiController class
 * @author xnovakf00
 * @date 07.05.2025
 */


#include "GuiController.h"
#include "../common/EMessageType.h"
#include <iostream>
#include "../networkHandler/NetworkHandler.h"
#include <QMetaObject>

GuiController::GuiController(IMainWindow* gui) {
    this->gui = gui;
    qDebug() << "GuiController created with gui pointer:" << gui;
}

void GuiController::performAction(Message &msg) {
    
    EMessageType type = msg.getType();

    safePrint( "Going to perform an action on type" + eMessageTypeToString(type));
    switch (type) {
        case (EMessageType::REJECT) : {
            this->gui->showError(msg.getOtherInfo());
            break;
        }

        case (EMessageType::LOG) : {
            
            EItemType activableType = msg.getElementType();
            std::string activableID = msg.getCurrentElement();

            IActivable& toActivate = this->gui->getActivableItem(activableType, activableID);

            auto outputs = msg.getOutputValues();

            for (const auto& output : outputs) {
                std::string name = output.first;
                std::string value = output.second;
            
                this->gui->showOutput(name, value);
            }
            auto inputs = msg.getInputValues();

            for (const auto& input : inputs) {
                std::string name = input.first;
                std::string value = input.second;
            
                this->gui->showInput(name, value);
            }
            this->gui->highlightItem(true, toActivate);
            std::string log = msg.getLogString();
            QMetaObject::invokeMethod(this, [log, this]() {
                this->gui->printLog(log);
            }, Qt::QueuedConnection);
            break;
        }

        case (EMessageType::ACCEPT) : {
            safePrint( "Yes, I am going to accept");
            this->gui->setRunning();
            safePrint( "Yes, I have accepted going to accept");
            break;
        }
        

        default:
            break;
    };
}

