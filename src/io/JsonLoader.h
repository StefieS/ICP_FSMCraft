#pragma once

#include "FSM.h"
#include <QJsonDocument>
class JsonLoader {
public:
    FSM* fromJson(const QJsonDocument& jsonDoc);
};