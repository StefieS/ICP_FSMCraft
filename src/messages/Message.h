/**
 * @file Message.h
 * @brief Header file for custom message to send through the network
 * @author xnovakf00
 * @date 05.05.2025
 */

#pragma once

#include <string>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include "../common/EMessageType.h"
#include "../common/EItemType.h"
#include <map>

/**
 * @class Message
 * @brief Holds information about all types of messages sent through custom protocol
 */
class Message {    
private:
    /** @brief The type of the message (e.g., JSON, INPUT, LOG, etc.). */
    EMessageType type;

    // JSON
    /** @brief Name associated with a JSON message. */
    std::string name;

    // INPUT
    /** @brief Name of the input signal. */
    std::string inputName;

    /** @brief Value of the input signal. */
    std::string inputValue;

    // LOG
    /** @brief Timestamp for a log message. */
    std::string timestamp;

    /** @brief The type of the FSM element associated with the message. */
    EItemType elementType;

    /** @brief Identifier of the currently processed element. */
    std::string currentElement;

    /** @brief Key-value pairs of input signals. */
    std::map<std::string, std::string> inputValues;

    /** @brief Key-value pairs of output signals. */
    std::map<std::string, std::string> outputValues;

    /** @brief Key-value pairs of internal FSM variables. */
    std::map<std::string, std::string> internalValues;

    /** @brief Additional metadata or information attached to the message. */
    std::string otherData;

public:
    /**
     * @brief Constructs a Message from a raw string representation.
     * @param receivedMessage The raw message string to parse.
     */
    Message(std::string receivedMessage);

    /**
     * @brief Default constructor.
     */
    Message();

    /**
     * @brief Serializes the message to a string format.
     * @return A string representing the full message.
     */
    std::string toMessageString() const;

    /**
     * @brief Constructs an input message with the given name and value.
     * @param inputName Name of the input.
     * @param inputValue Value of the input.
     */
    void buildInputMessage(const std::string& inputName, const std::string& inputValue);

    /**
     * @brief Constructs a JSON message with the given name.
     * @param jsonName Name associated with the JSON message.
     */
    void buildJsonMessage(const std::string& jsonName);

    /**
     * @brief Constructs a stop control message.
     */
    void buildStopMessage();

    /**
     * @brief Constructs an accept control message.
     */
    void buildAcceptMessage();

    /**
     * @brief Constructs a reject control message with additional info.
     * @param otherInfo Additional reason or metadata for rejection.
     */
    void buildRejectMessage(const std::string& otherInfo);

    /**
     * @brief Constructs a request message.
     */
    void buildRequestMessage();

    /**
     * @brief Constructs a log message with various telemetry data.
     * @param timestamp Time of the event.
     * @param elementType Type of the element involved.
     * @param currentElement Identifier of the element.
     * @param inputValues Key-value pairs of input values.
     * @param outputValues Key-value pairs of output values.
     * @param internalValues Key-value pairs of internal state values.
     */
    void buildLogMessage(const std::string& timestamp,
                         EItemType elementType,
                         const std::string& currentElement,
                         const std::map<std::string, std::string>& inputValues,
                         const std::map<std::string, std::string>& outputValues,
                         const std::map<std::string, std::string>& internalValues);

    /**
     * @brief Gets the type of the message.
     * @return The message type.
     */
    EMessageType getType() const;

    /**
     * @brief Gets the name associated with a JSON message.
     * @return The JSON message name.
     */
    std::string getJsonName() const;

    /**
     * @brief Gets the name of the input.
     * @return The input name.
     */
    std::string getInputName() const;

    /**
     * @brief Gets the value of the input.
     * @return The input value.
     */
    std::string getInputValue() const;

    /**
     * @brief Gets the formatted log message string.
     * @return The log string.
     */
    std::string getLogString() const;

    /**
     * @brief Gets the current element identifier.
     * @return The element identifier string.
     */
    std::string getCurrentElement() const;

    /**
     * @brief Gets the type of the associated element.
     * @return The element type.
     */
    EItemType getElementType() const;

    /**
     * @brief Gets a map of output values.
     * @return Output values as key-value pairs.
     */
    std::map<std::string, std::string> getOutputValues() const;

    /**
     * @brief Gets a map of input values.
     * @return Input values as key-value pairs.
     */
    std::map<std::string, std::string> getInputValues() const;

    /**
     * @brief Gets additional information attached to the message.
     * @return The other data string.
     */
    std::string getOtherInfo() const;
};