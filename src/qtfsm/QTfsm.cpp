#include "QTfsm.h"
#include <QDebug>
#include "QTTransition.h"
#include "QTBuiltinHandler.h"

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

void QTfsm::initializeJsEngine() {
    QTBuiltinHandler* builtinHandler = new QTBuiltinHandler();
    QJSValue builtinHandlerJs = engine.newQObject(builtinHandler);
    engine.globalObject().setProperty("fsm", builtinHandlerJs);
}

// TODO
void QTfsm::addStateJsAction(QState* state, const QString& jsCode) {
    qDebug() << "Added action with code:" << jsCode;

    QObject::connect(state, &QState::entered, this, [this, jsCode]() {


        QJSValue result = this->engine.evaluate(jsCode);
        if (result.isError()) {
            qWarning() << "JavaScript error in state entry action:" << result.toString();
        }

        // epsilon
        QString empty = "";
        QVariantMap map; // todo add current map
        this->postEvent(new JsConditionEvent(map, empty));

    });

    QObject::connect(state, &QState::exited, this, [state]() {
    for (auto* t : state->transitions()) {
        if (auto* jsT = dynamic_cast<JsConditionTransition*>(t)) {
            jsT->cancelDelayTimer();
        }
    }
    });

    
}

QAbstractState* QTfsm::getStateByName(const QString& name) const {
    const auto children = this->automaton->findChildren<QState*>(QString(), Qt::FindDirectChildrenOnly);
    for (QState* state : children) {
        if (state->objectName() == name) {
            return state;
        }
    }
    return nullptr; 
}

void QTfsm::setJsVariable(const QString& name, const QJSValue& value) {
    engine.globalObject().setProperty(name, value);
}


void QTfsm::addJsTransition(QState* from, QAbstractState* to, const QString& condition, const QString& expectedInput, const QString& timeout) {
    JsConditionTransition *trans = new JsConditionTransition(&this->engine, condition, expectedInput, from, timeout, this);

    trans->setTargetState(to);
    from->addTransition(trans);
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
    initializeJsEngine();
    machine.start();
}

void QTfsm::stop() {
    machine.stop();
}

QStateMachine* QTfsm::getMachine() {
    return &this->machine;
}
