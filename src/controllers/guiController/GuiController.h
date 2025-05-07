#pragma once
#include "IMainWindow.h"
#include "../messages/Message.h"
class GuiController {
private:
    IMainWindow* gui;

public:
    GuiController(IMainWindow* gui);
    void performAction(Message &msg);
};