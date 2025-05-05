#include "JsonLoader.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "FSM.h"

FSM* JsonLoader::fromJson(const QJsonDocument& jsonDoc) {
    
    QJsonObject root = jsonDoc.object();

    // initialize fsm with name
    FSM* fsm = new FSM(root["name"].toString().toStdString());

    // get inputs -> to which inputs the fsm will react
    QJsonArray inputs = root["inputs"].toArray();
    for (int i = 0; i < inputs.size(); ++i) {
        fsm->addInputName(inputs[i].toString().toStdString());
    }

    // get outputs -> to which outputs the fsm will react
    QJsonArray outputs = root["outputs"].toArray();
    for (int i = 0; i < outputs.size(); ++i) {
        fsm->addOutputName(outputs[i].toString().toStdString());
    }

    // get internal variables
    QJsonArray internals = root["internal"].toArray();
    for (int i = 0; i < internals.size(); ++i) {
        QJsonObject internalObj = internals[i].toObject();
        std::string name = internalObj["name"].toString().toStdString();
        std::string type = internalObj["type"].toString().toStdString();
        std::string initialValue = internalObj["initialValue"].toString().toStdString();
        auto internalVar = InternalVar();
        internalVar.setName(name);
        internalVar.setType(type);
        internalVar.setInitialValue(initialValue);
        fsm->addInternalVar(internalVar);
    }

    // get states -> states of the fsm
    QJsonArray states = root["states"].toArray();
    for (int i = 0; i < states.size(); ++i) {
        QJsonObject stateObj = states[i].toObject();
        std::string stateName = stateObj["name"].toString().toStdString();
        std::string actionCode = stateObj["action"].toString().toStdString();
        bool isInitial = stateObj["isInitial"].toBool();
        if (isInitial) {
            fsm->setInitialState(stateName);
        }
        bool isFinal = stateObj["isFinal"].toBool();
        std::shared_ptr<State> state = std::make_shared<State>(stateName, actionCode, isInitial, isFinal);
        fsm->addState(state);
    }

    // get transitions -> transitions of the fsm
    QJsonArray transitions = root["transitions"].toArray();
    for (int i = 0; i < transitions.size(); ++i) {
        QJsonObject transitionObj = transitions[i].toObject();
        std::string src = transitionObj["src"].toString().toStdString();
        std::string dst = transitionObj["dst"].toString().toStdString();
        std::string input = transitionObj["input"].toString().toStdString();
        std::string cond = transitionObj["cond"].toString().toStdString();
        std::shared_ptr<Transition> transition = std::make_shared<Transition>(src, dst, input, cond);
        fsm->addTransition(transition);
    }

    return fsm;
}