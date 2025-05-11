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
#include <QPushButton>
#include "IMainWindow.h"
#include "../io/JsonLoader.h"
#include "../networkHandler/NetworkHandler.h"
#include "../common/EMessageType.h"
#include "../controllers/guiController/GuiController.h"


class MainWindow : public QMainWindow, public IMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    IActivable& getActivableItem(EItemType type, std::string itemID) override;
    void printLog(std::string logMessage) override;
    void highlightItem(bool on, IActivable& item) override;
    void setRunning() override;
    void clearHighlights();
    void setInterfaceLocked(bool locked);
    void showError(std::string errorMessage) override;
    void showOutput(std::string outputID, std::string outputValue) override;
    void showInput(std::string inputID, std::string inputValue) override;
    void loadFSMFromJson(std::string pathToJson) override;
    

    std::thread listenerThread;
    std::thread recvThread;
    std::atomic<bool> listenerRunning = false;
    int TransitionId = 0;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onInjectInputClicked();
    void onNewStateButtonClicked();
    void onRunClicked();
    void onClearClicked();
    void onStopClicked();
    void onSaveClicked();
    void onUploadClicked();
    void sendInitialMessage();
    void startReceivingMessages();
    std::tuple<QString, QString, QString> askForTransitionDetails();
    std::pair<QString, QString> askForStateDetails();
    QString askToEditAction(const QString& currentCode);
    

private:
    // FSM engine
    FSM* fsm = nullptr;
    
    static const int MAX_STATES = 100;
    State* stateList[MAX_STATES];
    int stateCount = 0;
    bool connectingMode = false;
    bool isRunning = false;
    
    // Buttons
    QToolButton* runButton = nullptr;
    QToolButton* newStateButton = nullptr;
    
    StateItem* transitionStart = nullptr;
    TransitionItem* currentLine = nullptr;
    
    QWidget* inputListContainer;
    QVBoxLayout* inputListLayout;   

    // State label
    QLabel* stateLabel = nullptr;
    
    // Graphics scene and view
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

    // State creation logic
    bool addingNewState = false;
    QGraphicsEllipseItem* ghostCircle = nullptr;
    
    NetworkHandler networkHandler;
    NetworkHandler networkHandler2;
    bool connected = false;

    GuiController* controller = nullptr;
    IActivable* lastActive = nullptr;

    QString automatonName = "basic";
    // Helpers
    void debugPrintStateList() const;
};
