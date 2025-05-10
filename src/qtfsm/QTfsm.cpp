#include "QTfsm.h"
#include <QDebug>
#include "QTTransition.h"
#include "QTBuiltinHandler.h"
#include <QDateTime>
#include <QApplication>
#include "../common/EItemType.h"
#include "../messages/Message.h"

QTfsm::QTfsm(QObject* parent, const std::string& name) 
    : QObject(parent), jsonName(name), networkHandler("127.0.0.1", 8080), connected(false) {
    this->automaton = new QState(&machine);
    this->end = new QFinalState(&machine); 
    automaton->addTransition(this->automaton, &QState::finished, this->end);
    automaton->addTransition(this, &QTfsm::stopSignal, this->end);
    machine.setInitialState(this->automaton);
    this->moveToThread(QCoreApplication::instance()->thread());
    this->getMachine()->moveToThread(QCoreApplication::instance()->thread());
}

QState* QTfsm::addState(const QString& name) {
    if (!this->automaton) {
        qWarning() << "Automaton is null. Cannot add state.";
        return nullptr;
    }

    // Ensure we're in the same thread
    if (this->thread() != this->automaton->thread()) {
        qWarning() << "QTfsm and automaton are in different threads!";
    }

    QState* state = new QState(this->automaton);  // owned by automaton
    if (!name.isEmpty()) {
        state->setObjectName(name);
        qDebug() << "Added state:" << name;
    } else {
        qDebug() << "Added unnamed state";
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
    QTBuiltinHandler* builtinHandler = new QTBuiltinHandler(nullptr, this);
    this->builtinHandler = builtinHandler;
    QJSValue builtinHandlerJs = engine.newQObject(builtinHandler);
    engine.globalObject().setProperty("fsm", builtinHandlerJs);
}

// TODO
void QTfsm::addStateJsAction(QState* state, const QString& jsCode) {
    qDebug() << "Added action with code:" << jsCode;

    QObject::connect(state, &QState::entered, this, [this, jsCode, state]() {
        qDebug() << "STATE ENTERED";
        builtinHandler->stateEntered(state);
        QJSValue result = this->engine.evaluate(jsCode);
        if (result.isError()) {
            qWarning() << "JavaScript error in state entry action:" << result.toString();
        }
        // TODO send Logs
        // TimeStamp
        QDateTime now = QDateTime::currentDateTime();
        QString timeStr = now.toString("yyyy-MM-dd hh:mm:ss");  // or any format you want
        std::string timeStamp = timeStr.toStdString();
        // Elem State
        EItemType elementType = EItemType::STATE;
        // currentElem
        const std::string currentElement = state->objectName().toStdString();
        Message log = Message();
        log.buildLogMessage(timeStamp,
            elementType,
            currentElement,
            getStringMap(this->inputValues),
            getStringMap(this->outputValues),
            getStringMap(this->internalValues));

        this->networkHandler.sendToHost(log.toMessageString());
        // epsilon
        QString empty = "";
        QVariantMap map; // todo add current map
        this->postEvent(new JsConditionEvent(map, empty));

    }, Qt::QueuedConnection);

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
    this->internalValues[name.toStdString()] = value;
    engine.globalObject().setProperty(name, value);
}

void QTfsm::setOutput(const QString& name, const QJSValue& value) {
    this->outputValues[name.toStdString()] = value;
}
void QTfsm::setInput(const QString& name, const QJSValue& value) {
    this->inputValues[name.toStdString()] = value;
}


void QTfsm::addJsTransition(QState* from, QAbstractState* to, const QString& condition, const QString& expectedInput, const QString& timeout) {
    JsConditionTransition *trans = new JsConditionTransition(&this->engine, condition, expectedInput, from, timeout, this);

    trans->setTargetState(to);
    from->addTransition(trans);
}


void QTfsm::postEvent(QEvent* event) {
    qDebug() << "POSTED";
    getMachine()->postEvent(event);
}

std::string QTfsm::getName() {
    return this->jsonName;
}

void QTfsm::emitStopSignal() {
    emit stopSignal();
}

void QTfsm::start() {
    initializeJsEngine();
    this->connected = this->networkHandler.connectToServer();
    if (!this->connected) {
        qWarning() << "Failed to connect to host. State machine will not start.";
        return;
    }
    // Start state machine after moving it to the main thread
    QMetaObject::invokeMethod(&machine, "start", Qt::QueuedConnection);
}

void QTfsm::stop() {
    this->stopSignal();
    QMetaObject::invokeMethod(&machine, "stop", Qt::QueuedConnection);
    this->networkHandler.closeConnection();
}


QStateMachine* QTfsm::getMachine() {
    return &this->machine;
}

std::map<std::string, std::string> QTfsm::getStringMap(std::map<std::string, QJSValue> map) {
    std::map<std::string, std::string> newMap = {};

    for (auto i : map) {
        QString qstr = i.second.toString();
        newMap[i.first] = qstr.toStdString();
    }

    return newMap;
}


NetworkHandler& QTfsm::getNetworkHandler() {
    return this->networkHandler;
}

std::map<std::string, QJSValue> QTfsm::getInputs(){
    return this->inputValues;
}

std::map<std::string, QJSValue> QTfsm::getOutputs(){
    return this->outputValues;
}
std::map<std::string, QJSValue> QTfsm::getVars(){
    return this->internalValues;
}