#pragma once

#include <string>

class InternalVar {

private:
    std::string name;
    std::string type;
    std::string initialValue;

public:
    std::string setName(std::string name);
    std::string setType(std::string type);
    std::string setInitialValue(std::string initialValue);

    std::string getName() const;
    std::string getType();
    std::string getInitialValue() const;
};