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


class MainWindow : public QMainWindow, public IMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    IActivable& getActivableItem(EItemType type, std::string itemID) override;
    void printLog(std::string logMessage);
    void highlightItem(bool on, IActivable& item);
    void setRunning();
    void showError(std::string errorMessage);
    void showOutput(std::string outputID, std::string outputValue);
    void loadFSMFromJson(std::string pathToJson) override;

    std::thread listenerThread;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onInjectInputClicked();
    void onNewStateButtonClicked();
    void onRunClicked();
    void onClearClicked();
    void onStopClicked();
    void onSaveClicked();
    void onUploadClicked();
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
    bool connected = false;

    // Helpers
    void debugPrintStateList() const;
};
