#include "QTfsm.h"
#include <QDebug>

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

QFinalState* QTfsm::addFinalState() {
    QFinalState* finalState = new QFinalState(this->automaton);
    return finalState; 
}

void QTfsm::addStateActions(QAbstractState* state, std::function<void()> onEntry) {
    if (onEntry) {
        QObject::connect(state, &QState::entered, this, [onEntry]() {
            onEntry();
        });
    }
}

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
