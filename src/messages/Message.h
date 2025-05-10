#pragma once

#include <string>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include "../common/EMessageType.h"
#include "../common/EItemType.h"
#include <map>

// datum a čas, aktuální stav/transition, hodnoty vstupů, výstupů a interních proměnných.
class Message {    
private:
    EMessageType type;

    // JSON
    std::string name;

    // INPUT
    std::string inputName;
    std::string inputValue;

    // LOG
    std::string timestamp;

    EItemType elementType;
    std::string currentElement;
    
    std::map<std::string, std::string> inputValues;
    std::map<std::string, std::string> outputValues;
    std::map<std::string, std::string> internalValues;

    std::string otherData;

public:

    Message(std::string receivedMessage);
    Message();

    std::string toMessageString() const;

    void buildInputMessage(const std::string& inputName, const std::string& inputValue);
    void buildJsonMessage(const std::string& jsonName);
    void buildStopMessage();
    void buildAcceptMessage();
    void buildRejectMessage(const std::string& otherInfo);
    void buildLogMessage(const std::string& timestamp, 
        EItemType elementType,
        const std::string& currentElement, 
        const std::map<std::string, std::string>& inputValues, 
        const std::map<std::string, std::string>& outputValues, 
        const std::map<std::string, std::string>& internalValues);
    
    EMessageType getType() const;
    std::string getJsonName() const;
    std::string getInputName() const;
    std::string getInputValue() const;
    
    std::string getLogString() const;
    std::string getCurrentElement() const;
    EItemType getElementType() const;
    std::map<std::string, std::string> getOutputValues() const;
    std::map<std::string, std::string> getInputValues() const;
    std::string getOtherInfo() const;
};