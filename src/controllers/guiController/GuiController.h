/**
 * @file GuiController.h
 * @brief Header file for the GuiController class
 * @author xnovakf00
 * @date 07.05.2025
 */

#pragma once
#include "../../gui/IMainWindow.h"
#include "../../messages/Message.h"

/**
 * @class GuiController
 * @brief Class for controlling the GUI
 */
class GuiController : public QObject {
    Q_OBJECT
private:
    /**
     * GUI window to control
     */
    IMainWindow* gui;
public:
    /**
     * @brief Constructor for the controller
     * @param gui Window to control
     */
    GuiController(IMainWindow* gui);
    std::string buffer;
    /**
     * @brief Based on the received message type
     * performs action on GUI
     * 
     * @param msg Message to perform action based on
     */
    void performAction(Message &msg);
    void sendInput(const std::string& name, const std::string& value);
};