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
    // communication is too fast, we have to limit it
    std::this_thread::sleep_for(std::chrono::seconds(1)); 
    EMessageType type = msg.getType();
    std::cout << eMessageTypeToString(type) << "HIER";
    safePrint( "Going to perform an action on type" + eMessageTypeToString(type));
    switch (type) {
        case (EMessageType::REJECT) : {
            QMetaObject::invokeMethod(this, [=]() {
                this->gui->showError(msg.getOtherInfo());
            }, Qt::QueuedConnection);
            break;
        }

        case (EMessageType::LOG) : {
            
            EItemType activableType = msg.getElementType();
            std::string activableID = msg.getCurrentElement();
        
            QMetaObject::invokeMethod(this, [this, activableID, activableType]() {
                IActivable& toActivate = this->gui->getActivableItem(activableType, activableID);
                this->gui->highlightItem(true, toActivate);
                std::cout<< "*********************************" << activableID << " " << "activavle" << "***********************************\n";
            }, Qt::QueuedConnection);
            safePrint("GOT ACTIVE ELE");

            std::string log = msg.getLogString();
            QMetaObject::invokeMethod(this, [log, this]() {
                this->gui->printLog(log);
            }, Qt::QueuedConnection);

            auto outputs = msg.getOutputValues();

            for (const auto& output : outputs) {
                std::string name = output.first;
                std::string value = output.second;
            
                QMetaObject::invokeMethod(this, [this, name, value]() {
                    this->gui->showOutput(name, value);
                }, Qt::QueuedConnection);
            }
            auto inputs = msg.getInputValues();

            for (const auto& input : inputs) {
                std::string name = input.first;
                std::string value = input.second;
                QMetaObject::invokeMethod(this, [this, name, value]() {
                    this->gui->showInput(name, value);
                }, Qt::QueuedConnection);
            }

            break;
        }

        case (EMessageType::ACCEPT) : {
            safePrint( "Yes, I am going to accept");
            QMetaObject::invokeMethod(this, [=]() {
                this->gui->setRunning();
            }, Qt::QueuedConnection);
            safePrint( "Yes, I have accepted going to accept");
            break;
        }

        
        case (EMessageType::JSON): {
            QMetaObject::invokeMethod(this, [=]() {
                this->gui->loadFSMFromJson(msg.getJsonName() + ".json");
            }, Qt::QueuedConnection);
            break;
        }

        default:
            break;
    };
}
