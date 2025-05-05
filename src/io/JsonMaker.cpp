/**
*  @file JsonMaker.cpp
 * @brief Implementation file for the JsonMaker class for creating Json representation
 * of the FSM
 * @author xnovakf00
 * @date 05.05.2025
 */


#include "JsonMaker.h"
using namespace std;

QJsonObject JsonMaker::toJson(FSM* fsm) {
  QJsonObject doc;


  string name = fsm->getName();
  doc["name"] = QString::fromStdString(name);
  
  QJsonArray inputsArr;
  for (string& input : fsm->getInputNames()) {
    inputsArr.append(QString::fromStdString(input));
  }
  doc["inputs"] = inputsArr;

  QJsonArray outputsArr;
  for (string& output : fsm->getOutputNames()) {
    outputsArr.append(QString::fromStdString(output));
  }
  doc["outputs"] = outputsArr;

  QJsonArray internalsArr;
  for (InternalVar internal : fsm->getInternalVars()) {
    QJsonObject internalObj;
    internalObj["name"] = QString::fromStdString(internal.getName());
    internalObj["type"] = QString::fromStdString(internal.getType());
    internalObj["initial"] = QString::fromStdString(internal.getInitialValue());
    internalsArr.append(internalObj);
  }
  doc["internals"] = internalsArr;

  QJsonArray statesArr;
  for (auto& state : fsm->getStates()) {
    QJsonObject stateObj;
    stateObj["name"] = QString::fromStdString(state.first);
    stateObj["action"] = QString::fromStdString(state.second->getActionCode());
    stateObj["isInitial"] = state.second->isInitialState();
    stateObj["isFinal"] = state.second->isFinalState();
    statesArr.append(stateObj);
  }
  doc["states"] = statesArr;

  QJsonArray transitionsArr;
  for (auto& transition : fsm->getTransitions()) {
    QJsonObject transitionObj;
    transitionObj["src"] = QString::fromStdString(transition->getSource());
    transitionObj["dst"] = QString::fromStdString(transition->getTarget());
    transitionObj["input"] = QString::fromStdString(transition->getInputEvent());
    transitionObj["cond"] = QString::fromStdString(transition->getGuardCondition());
    transitionsArr.append(transitionObj);
  }
  doc["transitions"] = transitionsArr;

  return doc;
}