/**
 * @file MainWindow.h
 * @brief Main GUI window for FSMCraft: allows editing, simulation, and monitoring of finite state machines.
 * 
 * This class integrates the user interface logic and visual state diagram editing tools,
 * allowing the user to build FSMs visually, simulate them in real time, and monitor inputs/outputs.
 * It connects to a running interpreter via sockets and displays current state and transitions.
 * 
 * Authors: xmarina00, xnovakf00, xlesigm00
 */

#pragma once

#include "State.h"
#include "StateItem.h"
#include "TransitionItem.h"
#include <QMainWindow>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QSet>
#include <QMap>
#include "InternalVarItem.h"
#include "QFlowLayout.h"
#include "FSM.h"
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include "IMainWindow.h"
#include "../io/JsonLoader.h"
#include "../networkHandler/NetworkHandler.h"
#include "../common/EMessageType.h"
#include "../controllers/guiController/GuiController.h"

/**
 * @class MainWindow
 * @brief GUI interface and controller for the FSM visual editor and interpreter monitor.
 */
class MainWindow : public QMainWindow, public IMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructs the main window and sets up UI components.
     * @param parent Parent widget (typically null).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor. Cleans up FSM and GUI state.
     */
    ~MainWindow();

    /**
     * @brief Finds and returns a reference to a visual or logical item by type and ID.
     * @param type Type of item (state or transition).
     * @param itemID ID or label of the item to find.
     * @return Reference to IActivable item (state or transition).
     */
    IActivable& getActivableItem(EItemType type, std::string itemID) override;

    /**
     * @brief Appends a message to the debug log view.
     * @param logMessage Text message to log.
     */
    void printLog(std::string logMessage) override;

    /**
     * @brief Highlights or unhighlights a state or transition in the diagram.
     * @param on Whether to activate (true) or deactivate (false) highlighting.
     * @param item The item to highlight.
     */
    void highlightItem(bool on, IActivable& item) override;

    /**
     * @brief Called when the FSM is set into the running state.
     */
    void setRunning() override;

    /**
     * @brief Clears all highlights from states and transitions.
     */
    void clearHighlights();

    /**
     * @brief Enables or disables parts of the UI while running the FSM.
     * @param locked Whether to lock or unlock user controls.
     */
    void setInterfaceLocked(bool locked);

    /**
     * @brief Displays an error message to the user in a dialog.
     * @param errorMessage Message string to display.
     */
    void showError(std::string errorMessage) override;

    /**
     * @brief Updates the value of a monitored output in the GUI.
     * @param outputID ID of the output.
     * @param outputValue New value as string.
     */
    void showOutput(std::string outputID, std::string outputValue) override;

    /**
     * @brief Updates the value of an input field in the GUI.
     * @param inputID ID of the input.
     * @param inputValue New value as string.
     */
    void showInput(std::string inputID, std::string inputValue) override;

    /**
     * @brief Loads a saved FSM model from a JSON file.
     * @param pathToJson Relative path to the JSON file.
     */
    void loadFSMFromJson(std::string pathToJson) override;

    std::thread listenerThread;   ///< Thread listening for runtime messages
    std::thread recvThread;       ///< Additional thread for receiving async messages
    std::atomic<bool> listenerRunning = false; ///< Whether the listener is active
    int TransitionId = 1;         ///< ID counter for transitions

protected:
    /**
     * @brief Event filter for intercepting mouse and UI events.
     * @param watched The QObject being watched.
     * @param event The intercepted event.
     * @return True if event is handled; false otherwise.
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onInjectInputClicked();         ///< Slot for handling input injection
    void onNewStateButtonClicked();      ///< Slot for creating a new state
    void onRunClicked();                 ///< Slot for toggling FSM run mode
    void onClearClicked();               ///< Slot for clearing the diagram
    void onStopClicked();                ///< Slot for stopping the FSM
    void onSaveClicked();                ///< Slot for saving the FSM
    void onUploadClicked();              ///< Slot for uploading and loading a saved FSM
    void sendInitialMessage();           ///< Sends initial request to backend
    void startReceivingMessages();       ///< Begins asynchronous message reception

    /**
     * @brief Prompts the user to define transition properties.
     * @return Tuple of (input name, guard condition, timeout).
     */
    std::tuple<QString, QString, QString> askForTransitionDetails();

    /**
     * @brief Prompts the user to define a new state (name and action).
     * @return Pair of (state name, action block).
     */
    std::pair<QString, QString> askForStateDetails();

    /**
     * @brief Prompts the user to edit the action code of a state.
     * @param currentCode Current code in the state.
     * @return Edited code as a string.
     */
    QString askToEditAction(const QString& currentCode);

private:
    FSM* fsm = nullptr;               ///< Pointer to the FSM logic model
    static const int MAX_STATES = 100;
    State* stateList[MAX_STATES];     ///< Fixed-size array of state logic
    int stateCount = 0;               ///< Number of currently added states
    bool connectingMode = false;      ///< Whether user is creating a transition
    bool isRunning = false;           ///< Whether FSM is running

    // Buttons and input
    QToolButton* runButton = nullptr;
    QToolButton* newStateButton = nullptr;

    StateItem* transitionStart = nullptr;    ///< Pointer to state where transition started
    TransitionItem* currentLine = nullptr;   ///< Pointer to temporary transition line

    QWidget* inputListContainer;
    QVBoxLayout* inputListLayout;

    QLabel* stateLabel = nullptr;

    // Graphics
    QGraphicsScene* scene = nullptr;
    QGraphicsView* view = nullptr;
    QWidget* internalVarsContainer;
    QFlowLayout* internalVarsFlow;
    QMap<QString, InternalVarItem*> internalVarMap;

    QMap<QString, QLineEdit*> inputMap;
    QComboBox* inputComboBox = nullptr;
    QLineEdit* inputValueEdit = nullptr;
    QPushButton* injectInputButton = nullptr;

    QMap<QString, QLineEdit*> outputMap;
    QWidget* outputListContainer;
    QVBoxLayout* outputListLayout;

    QWidget* inputInjectionContainer = nullptr;
    QMap<QString, QLineEdit*> inputFieldMap;

    QPlainTextEdit* logBox;

    // FSM creation and input tracking
    bool addingNewState = false;
    QGraphicsEllipseItem* ghostCircle = nullptr;

    NetworkHandler networkHandler;     ///< Socket client to simulator
    NetworkHandler networkHandler2;    ///< Secondary connection for async data
    bool connected = false;

    GuiController* controller = nullptr;
    IActivable* lastActive = nullptr;

    QString automatonName = "basic";

    /**
     * @brief Outputs debug information about current states.
     */
    void debugPrintStateList() const;
};
