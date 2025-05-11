/**
 * @file IMainWindow.h
 * @brief Header file for the IMainWindow interface
 * @author xnovakf00
 * @date 06.05.2025
 */

#pragma once

#include "IActivable.h"
#include "../common/EItemType.h"
#include <string>
#include "../networkHandler/NetworkHandler.h"

/**
 * @brief IMainWindow interface
 * Used for the main window of the FSM GUI by controller
 * to show actions in the GUI
 */
class IMainWindow {
public:
    virtual ~IMainWindow() = default;
    
    /**
     * @brief Set the active state of the object, in GUI means to highlight it
     * or turn the highlight off.
     * @param on True to activate, false to deactivate.
     * @param item The object to highlight.
     */
    virtual void highlightItem(bool on, IActivable& item) = 0;

    /**
     * @brief Print the log.
     */
    virtual void printLog(std::string logMessage) = 0;

    /**
     * @brief Show the output of the FSM.
     * @param outputID The ID of the output to show.
     * @param outputValue The value of the output to show.
     */
    virtual void showOutput(std::string outputID, std::string outputValue) = 0;

    /**
     * @brief Show the input of the FSM.
     * @param inputID The ID of the input to show.
     * @param inputValue The value of the input to show.
     */
    virtual void showInput(std::string inputID, std::string inputValue) = 0;


    /**
     * @brief Shows an error message on screen.
     * @param errorMessage The message to show.
     */
    virtual void showError(std::string errorMessage) = 0;

    /**
     * @brief After receiving the ACCEPT message from server,
     * GUI is now ready to send inputs/expect outputs and is not editable.
     * 
     * @note To access editing, it has to be stopped.
     */
    virtual void setRunning() = 0;

    /**
     * @brief Returns a reference activable item based on the type and its id.
     * 
     * @param type Type of the item STATE/TRANSITION
     * @param itemID Id of the item to return.
     * 
     * @return A reference to the activable item wanted.
     */
    virtual IActivable& getActivableItem(EItemType type, std::string itemID) = 0;


    /**
     * @brief Loads json to GUI and shows it.
     * 
     * @param pathToJson Path to Json file
     */
    virtual void loadFSMFromJson(std::string pathToJson) = 0;

};