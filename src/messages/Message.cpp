#include "Message.h"
#include "../common/EMessageType.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <string>

Message::Message() {
    this->type = EMessageType::EMPTY;
    this->name = "";
    this->inputName = "";
    this->inputValue = "";
    this->timestamp = "";
    this->elementType = EItemType::STATE;
    this->currentElement = "";
    this->inputValues = {};
    this->outputValues = {};
    this->internalValues = {};
    this->otherData = {};
}

Message::Message(std::string receivedMessage) {
    const QJsonDocument& receivedMessageDoc = QJsonDocument::fromJson(QString::fromStdString(receivedMessage).toUtf8());

    QJsonObject root = receivedMessageDoc.object();
    this->type = messageTypeFromString(root["type"].toString().toStdString());

    switch (this->type) {
        case (EMessageType::INPUT): {
            std::string inputName = root["inputName"].toString().toStdString();
            std::string inputValue = root["inputValue"].toString().toStdString();
            this->buildInputMessage(inputName, inputValue);
            break;
        }
        case (EMessageType::JSON): {
            std::string jsonName = root["jsonName"].toString().toStdString();
            this->buildJsonMessage(jsonName);
            break;
        }
        case (EMessageType::STOP): {
            this->buildStopMessage();
            break;
        }
        case (EMessageType::ACCEPT): {
            this->buildAcceptMessage();
            break;
        }
        case (EMessageType::REJECT): {
            std::string otherInfo = root["otherInfo"].toString().toStdString();
            this->buildRejectMessage(otherInfo);
            break;
        }
        case (EMessageType::LOG): {
            std::string timestamp = root["timestamp"].toString().toStdString();
            std::string elementTypeStr = root["elementType"].toString().toStdString();
            EItemType elementType = elementTypeFromString(elementTypeStr);
            std::string currentElement = root["currentElement"].toString().toStdString();
            std::map<std::string, std::string> inputValues = {};
            QJsonArray inputs = root["inputs"].toArray();
            for (const QJsonValue& element : inputs) {
                std::string inputID = element["input"].toString().toStdString();
                std::string inputVal = element["value"].toString().toStdString();
                inputValues[inputID] = inputVal;
            }

            std::map<std::string, std::string> outputValues = {};
            QJsonArray outputs = root["outputs"].toArray();
            for (const QJsonValue& element : outputs) {
                std::string outputID = element["output"].toString().toStdString();
                std::string outputVal = element["value"].toString().toStdString();
                inputValues[outputID] = outputVal;
            }

            std::map<std::string, std::string> internalValues = {};
            QJsonArray internals = root["internals"].toArray();
            for (const QJsonValue& element : internals) {
                std::string internalID = element["var"].toString().toStdString();
                std::string internalVal = element["value"].toString().toStdString();
                inputValues[internalID] = internalVal;
            }

            this->buildLogMessage(timestamp,
                elementType,
                currentElement,
                inputValues,
                outputValues,
                internalValues);
            break;
        }
    }
}

void Message::buildInputMessage(const std::string& inputName, const std::string& inputValue) {
    this->type = EMessageType::INPUT;
    this->inputName = inputName;
    this->inputValue = inputValue;
}

void Message::buildJsonMessage(const std::string& jsonName) {
    this->type = EMessageType::JSON;
    this->name = jsonName;
}

void Message::buildAcceptMessage() {
    this->type = EMessageType::ACCEPT;
}

void Message::buildRejectMessage(const std::string& otherInfo) {
    this->type = EMessageType::REJECT;
    this->otherData = otherInfo;
}

void Message::buildStopMessage() {
    this->type = EMessageType::STOP;
}
void Message::buildLogMessage(const std::string& timestamp,
    EItemType elementType,
    const std::string& currentElement, 
    const std::map<std::string, std::string>& inputValues, 
    const std::map<std::string, std::string>& outputValues, 
    const std::map<std::string, std::string>& internalValues) {
        
        this->timestamp = timestamp;
        this->elementType = elementType;
        this->inputValues = inputValues;
        this->outputValues = outputValues;
        this->internalValues = internalValues;
    }

EMessageType Message::getType() const {
    return this->type;
}

std::string Message::getJsonName() const {
    return this->name;
}

std::string Message::getInputName() const {
    return this->inputName;
}
std::string Message::getInputValue() const {
    return this->inputValue;
}

EItemType Message::getElementType() const {
    return this->elementType;
}

std::string Message::getCurrentElement() const {
    return this->currentElement;
}

std::string Message::getOtherInfo() const {
    return this->otherData;
}

std::map<std::string, std::string> Message::getOutputValues() const {
    return this->outputValues;
}

std::string Message::getLogString() const {
    // TODO implement this
    return "";
}


