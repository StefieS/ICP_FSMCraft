/**
 * @file EMessageType.h
 * @brief Header file for the EItemType enumeration
 * @author xnovakf00
 * @date 06.05.2025
 */

 #pragma once

 #include <string>
 #include <stdexcept>
 
 /**
  * @brief Enumeration for message types
  */
 enum class EMessageType {
    LOG,
    INPUT,
    STOP,
    JSON,
    ACCEPT,
    REJECT,
    EMPTY,
    REQUEST,
 };
 
 /**
  * @brief Convert EMessageType to string.
  * @param type The EMessageType to convert.
  * @return String of the EMessageType.
  */
 inline std::string eMessageTypeToString(EMessageType type) {
     switch (type) {
        case EMessageType::LOG: return "LOG";
        case EMessageType::INPUT: return "INPUT";
        case EMessageType::STOP: return "STOP";
        case EMessageType::JSON: return "JSON";
        case EMessageType::ACCEPT: return "ACCEPT";
        case EMessageType::REJECT: return "REJECT";
        case EMessageType::EMPTY: return "EMPTY";
        case EMessageType::REQUEST: return "REQUEST";
        default: return "UNKNOWN";
     }
 }
 
 /**
  * @brief Convert string to EMessageType.
  * @param str The string to convert.
  * @return The corresponding EMessageType.
  * @throws std::invalid_argument if the string does not match any EMessageType.
  */
 inline EMessageType messageTypeFromString(const std::string& str) {
    if (str == "LOG") return EMessageType::LOG;
    if (str == "INPUT") return EMessageType::INPUT;
    if (str == "STOP") return EMessageType::STOP;
    if (str == "JSON") return EMessageType::JSON;
    if (str == "ACCEPT") return EMessageType::ACCEPT;
    if (str == "REJECT") return EMessageType::REJECT;
    if (str == "EMPTY") return EMessageType::EMPTY;
    if (str == "REQUEST") return EMessageType::REQUEST;
    return EMessageType::EMPTY;
 }