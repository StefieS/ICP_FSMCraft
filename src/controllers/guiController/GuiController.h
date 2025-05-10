/**
 * @file GuiController.h
 * @brief Header file for the GuiController class
 * @author xnovakf00
 * @date 07.05.2025
 */


#pragma once
#include "IMainWindow.h"
#include "../messages/Message.h"
class GuiController {
private:
    IMainWindow* gui;

public:
    GuiController(IMainWindow* gui);
    void performAction(Message &msg);
    void sendInput(const std::string& name, const std::string& value);
};