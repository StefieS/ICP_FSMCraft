/**
*  @file QTfsmBuilder.cpp
 * @brief Implementation file for a class to build QTFsm from loaded jsonDoc
 * @author xnovakf00
 * @date 08.05.2025
 */

#include "QTfsm.h"
#include "QTfsmBuilder.h"
#include "../fsm/State.h"
#include <utility>
#include "../fsm/Transition.h"
void QTfsmBuilder::buildQTfsm(const QJsonDocument& jsonDoc) {
    JsonLoader loader = JsonLoader();
    this->innerFsm = loader.fromJson(jsonDoc);

    this->built = new QTfsm(nullptr, this->innerFsm->getName());
    auto states = this->innerFsm->getStates();
    for (auto state : states) {
        QString stateName = QString::fromStdString(state.first);
        QString stateAction = QString::fromStdString(state.second->getActionCode());
        QState* toBeSet;
        if (state.second->isFinalState()) {
            this->built->addFinalState(stateName);
            return;
        } else if (state.second->isInitialState()) {
            qDebug("Added initial");
            toBeSet = this->built->addState(stateName);
            this->built->setInitialState(toBeSet);
        } else {
            toBeSet = this->built->addState(stateName);
        }
        this->built->addStateJsAction(toBeSet, stateAction);
    }

    auto transitions = this->innerFsm->getTransitions();
    for (auto transition : transitions) {
        QString srcName = QString::fromStdString(transition->getSource());
        QString trgtName = QString::fromStdString(transition->getTarget());
        QString cond = QString::fromStdString(transition->getGuardCondition());
        QString input = QString::fromStdString(transition->getInputEvent());
        QString timeout = QString::fromStdString(transition->getDelay());
        QState* srcState = qobject_cast<QState*>(this->built->getStateByName(srcName));
        QAbstractState* trgtState = this->built->getStateByName(trgtName);
        this->built->addJsTransition(srcState, trgtState, cond, input, timeout);
    }

    auto variables = this->innerFsm->getInternalVars();
    for (auto var : variables) {
        QJSValue val = this->built->getJsEngine()->toScriptValue(QString::fromStdString(var.getInitialValue()));
        QString name = QString::fromStdString(var.getName());
        this->built->setJsVariable(name, val);
    }

}

 QTfsm* QTfsmBuilder::getBuiltFsm() {
    return this->built;
 }
