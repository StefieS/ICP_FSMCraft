#include "GuiController.h"
#include "../common/EMessageType.h"
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
            // TODO other actions based on the type (show output/activate)
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