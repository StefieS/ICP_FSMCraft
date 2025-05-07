/**
 * @file EItemType.h
 * @brief Header file for the EItemType enumeration
 * @author xnovakf00
 * @date 06.05.2025
 */

#pragma once

#include <string>
#include <stdexcept>

/**
 * @brief Enumeration for item types in the FSM GUI.
 * Used to differentiate between states and transitions.
 */
enum class EItemType {
    STATE,
    TRANSITION
};

/**
 * @brief Convert EItemType to string.
 * @param type The EItemType to convert.
 * @return String of the EItemType.
 */
inline std::string eItemTypeToString(EItemType type) {
    switch (type) {
        case EItemType::STATE: return "STATE";
        case EItemType::TRANSITION: return "TRANSITION";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Convert string to EItemType.
 * @param str The string to convert.
 * @return The corresponding EItemType.
 * @throws std::invalid_argument if the string does not match any EItemType.
 */
inline EItemType elementTypeFromString(const std::string& str) {
    if (str == "STATE") return EItemType::STATE;
    if (str == "TRANSITION") return EItemType::TRANSITION;
    throw std::invalid_argument("Invalid EItemType string: " + str);
}