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

    QTfsm* qtfsm = new QTfsm(nullptr, this->innerFsm->getName());

    auto states = this->innerFsm->getStates();
    for (auto state : states) {
        QString stateName = QString::fromStdString(state.first);
        QString stateAction = QString::fromStdString(state.second->getActionCode());
        QAbstractState* toBeSet;
        if (state.second->isFinalState()) {
            toBeSet = qtfsm->addFinalState(stateName);
        } else if (state.second->isInitialState()) {
            toBeSet = qtfsm->addState(stateName);
            qtfsm->setInitialState(toBeSet);
        } else {
            toBeSet = qtfsm->addState(stateName);
        }
        qtfsm->addStateJsAction(toBeSet, stateAction);
    }

    auto transitions = this->innerFsm->getTransitions();
    for (auto transition : transitions) {
        QString srcName = QString::fromStdString(transition->getSource());
        QString trgtName = QString::fromStdString(transition->getTarget());
        QString cond = QString::fromStdString(transition->getGuardCondition()); 
        QState* srcState = qobject_cast<QState*>(qtfsm->getStateByName(srcName));
        QAbstractState* trgtState = qtfsm->getStateByName(trgtName);
        qtfsm->addJsTransition(srcState, trgtState, cond);
    }

    auto variables = this->innerFsm->getInternalVars();
    for (auto var : variables) {
        QJSValue val = qtfsm->getJsEngine()->toScriptValue(QString::fromStdString(var.getInitialValue()));
        QString name = QString::fromStdString(var.getName());
        qtfsm->setJsVariable(name, val);
    }

    this->built = qtfsm;
}
