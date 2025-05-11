/**
 * @file JsonLoader.h
 * @brief Header file for the JsonLoader class for creating a FSM from a Json representation
 * @author xnovakf00
 * @date 05.05.2025
 */

#pragma once

#include "../fsm/FSM.h"
#include <QJsonDocument>

/**
 * @class JsonLoader
 * @brief Class for loading a json into FSM
 */
class JsonLoader {
public:
    /**
     * @brief Builds a FSM based on the loaded json doc
     * @param jsonDoc loaded json document of the FSM
     * @return The transofrmed FSM
     */
    FSM* fromJson(const QJsonDocument& jsonDoc);
};