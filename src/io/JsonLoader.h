/**
 * @file JsonLoader.h
 * @brief Header file for the JsonLoader class for creating a FSM from a Json representation
 * @author xnovakf00
 * @date 05.05.2025
 */

#pragma once

#include "FSM.h"
#include <QJsonDocument>
class JsonLoader {
public:
    FSM* fromJson(const QJsonDocument& jsonDoc);
};