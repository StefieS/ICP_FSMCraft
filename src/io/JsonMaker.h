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


/**
 * @class JsonMaker
 * @brief Implements making of json document from inner fsm
 */
class JsonMaker {
public:
	/**
	 * @brief Builds json representation of the fsm
	 * @param fsm The fsm to transform
	 * @return Json object representing the FSM
	 */
	QJsonObject toJson(FSM* fsm);
};

