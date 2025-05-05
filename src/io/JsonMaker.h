/**
 * @file JsonMaker.h
 * @brief Header file for the JsonMaker class for creating Json representation
 * of the FSM
 * @author xnovakf00
 * @date 05.05.2025
 */
#pragma once

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "FSM.h"



class JsonMaker {
public:

	QJsonObject toJson(FSM* fsm);
};

