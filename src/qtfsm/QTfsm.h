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
    void addStateJsAction(QState* state, const QString& jsCode);
    void addJsTransition(QState* from, QState* to, const QString& jsCondition);
    // getters
    std::string getName();
    QJSEngine* getJsEngine();
    QStateMachine* getMachine();
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