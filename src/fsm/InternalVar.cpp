/**
 * @file InternalVar.cpp
 * @brief Implements the InternalVar class representing a variable internal to the FSM.
 * @author xnovakf00
 * @date 07.05.2025
 */


#include "InternalVar.h"
std::string InternalVar::setName(std::string name) {
    this->name = name;
    return this->name;
}

std::string InternalVar::setType(std::string type) {
    this->type = type;
    return this->type;
}

std::string InternalVar::setInitialValue(std::string initialValue) {
    this->initialValue = initialValue;
    return this->initialValue;
}

std::string InternalVar::getName() const {
    return name;
}

std::string InternalVar::getType() {
    return this->type;
}

std::string InternalVar::getInitialValue() const {
    return initialValue;
}
