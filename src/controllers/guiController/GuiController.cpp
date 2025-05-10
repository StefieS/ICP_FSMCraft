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

GuiController::GuiController(IMainWindow* gui) {
    this->gui = gui;
}

void GuiController::performAction(Message &msg) {

    EMessageType type = msg.getType();

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
            // TODO MAYBE SHOW INTERNAL
            this->gui->highlightItem(true, toActivate);
            this->gui->printLog(msg.getLogString());
            break;
        }

        case (EMessageType::ACCEPT) : {
            this->gui->setRunning();
            break;
        }
        

        default:
            break;
    };
}

void GuiController::sendInput(const std::string& name, const std::string& value) {
    // TODO: Replace this with actual message sending logic
    std::cout << "[SEND INPUT] " << name << " = " << value << std::endl;
}
