#include "QTfsm.h"
#include <QDebug>
#include "QTTransition.h"
QTfsm::QTfsm(QObject* parent, const std::string& name) 
    : QObject(parent), jsonName(name) {
    this->automaton = new QState(&machine);
    this->end = new QFinalState(&machine); 
    automaton->addTransition(this->automaton, &QState::finished, this->end);
    automaton->addTransition(this, &QTfsm::stopSignal, this->end);

    machine.setInitialState(this->automaton);
}

QState* QTfsm::addState(const QString& name) {
    QState* state = new QState(this->automaton); 
    if (!name.isEmpty()) {
        state->setObjectName(name);
    }
    return state;
}

void QTfsm::setInitialState(QAbstractState* state) {
    this->automaton->setInitialState(state);
}

QFinalState* QTfsm::addFinalState(const QString& name) {
    QFinalState* finalState = new QFinalState(this->automaton);
    if (!name.isEmpty()) {
        finalState->setObjectName(name);
    }
    return finalState; 
}

QJSEngine* QTfsm::getJsEngine(){
    return &this->engine;
}

// TODO
void QTfsm::addStateJsAction(QState* state, const QString& jsCode) {
    QObject::connect(state, &QState::entered, this, [this, jsCode]() {
        QJSValue result = this->engine.evaluate(jsCode);
        if (result.isError()) {
            qWarning() << "JavaScript error in state entry action:" << result.toString();
        }
    });
}

// Might be good
void QTfsm::addJsTransition(QState* from, QState* to, const QString& jsCondition) {
    auto* transition = new JsConditionTransition(&this->engine, jsCondition, from);
    transition->setTargetState(to);
    from->addTransition(transition);
}

// TODO: Handle input
// ...


void QTfsm::postEvent(QEvent* event) {
    machine.postEvent(event);
}

std::string QTfsm::getName() {
    return this->jsonName;
}

void QTfsm::emitStopSignal() {
    emit stopSignal();
}

void QTfsm::start() {
    machine.start();
}

void QTfsm::stop() {
    machine.stop();
}

QStateMachine* QTfsm::getMachine() {
    return &this->machine;
}
