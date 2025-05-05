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

std::string InternalVar::getName() {
    return this->name;
}

std::string InternalVar::getType() {
    return this->type;
}

std::string InternalVar::getInitialValue() {
    return this->initialValue;
}

