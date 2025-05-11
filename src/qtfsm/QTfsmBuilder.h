/**
*  @file QTfsmBuilder.h
 * @brief Header file for a class to build QTFsm from loaded jsonDoc
 * @author xnovakf00
 * @date 08.05.2025
 */

#pragma once
#include "../fsm/FSM.h"
#include "../fsm/State.h"
#include "../fsm/Transition.h"
#include "../io/JsonLoader.h"
#include "QTfsm.h"
class QTfsmBuilder {

private:
    FSM* innerFsm;
    QTfsm* built;

public:
    /**
     * @brief Builds QTfsm from json representation of the FSM.
     * @param jsonDoc json representation
     * @returns True if no problem, false if problem
     */
    bool buildQTfsm(const QJsonDocument& jsonDoc);

    /**
     * @brief Gets the built qtfsm
     * @return built qtfsm
     */
    QTfsm* getBuiltFsm();
};