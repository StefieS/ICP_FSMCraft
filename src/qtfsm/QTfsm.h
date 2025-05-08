#pragma once
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QObject>
#include <QJSEngine>

class QTfsm : public QObject {
    Q_OBJECT

public:
    QTfsm(QObject* parent = nullptr, const std::string& name = "");

    void start();
    void stop();
    void emitStopSignal();
    // Setup methods
    QState* addState(const QString& name = QString()); // Optionally name the state        
    void setInitialState(QAbstractState* state);
    QFinalState* addFinalState(const QString& name);
    void addStateJsAction(QAbstractState* state, const QString& jsCode);
    void addJsTransition(QState* from, QState* to, const QString& jsCondition);
    void setJsVariable(const QString& name, const QJSValue& value);
    // getters
    std::string getName();
    QJSEngine* getJsEngine();
    QStateMachine* getMachine();
    QState* getStateByName(const QString& name) const;
    void postEvent(QEvent* event);
signals:
    void stopSignal();

private:
    std::string jsonName;
    QStateMachine machine;
    QState* automaton;
    QFinalState* end;
    QJSEngine engine;
};