#pragma once
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QObject>
#include <QJSEngine>
#include "../networkHandler/NetworkHandler.h"
#include "QTBuiltinHandler.h"
class QTBuiltinHandler; // forward delcar
class QTfsm : public QObject {
    Q_OBJECT

public:
    QTfsm(QObject* parent = nullptr, const std::string& name = "");

    void start();
    void stop();
    void emitStopSignal();
    // Setup methods
    QState* addState(const QString& name);      
    void setInitialState(QAbstractState* state);
    QFinalState* addFinalState(const QString& name);
    void addStateJsAction(QState* state, const QString& jsCode);
    void addJsTransition(QState* from, 
        QAbstractState* to, 
        const QString& jsCondition, 
        const QString& expectedInput, 
        const QString& timeout);
    void setJsVariable(const QString& name, const QJSValue& value);
    void setOutput(const QString& name, const QJSValue& value);
    void setInput(const QString& name, const QJSValue& value);
    void initializeJsEngine();
    // getters
    std::string getName();
    QJSEngine* getJsEngine();
    QStateMachine* getMachine();
    QAbstractState* getStateByName(const QString& name) const;
    NetworkHandler& getNetworkHandler();
    std::map<std::string, QJSValue> getInputs();
    std::map<std::string, QJSValue> getOutputs();
    std::map<std::string, QJSValue> getVars();
    std::map<std::string, std::string> getStringMap(std::map<std::string, QJSValue> map);
    void postEvent(QEvent* event);
signals:
    void stopSignal();

private:
    std::string jsonName;
    bool connected;
    QStateMachine machine;
    QState* automaton;
    QFinalState* end;
    QJSEngine engine;
    NetworkHandler networkHandler;
    std::map<std::string, QJSValue> outputValues;
    std::map<std::string, QJSValue> internalValues;
    std::map<std::string, QJSValue> inputValues;
    QTBuiltinHandler* builtinHandler;
};