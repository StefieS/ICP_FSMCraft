/**
 * @file InternalVar.h
 * @brief Declares the InternalVar class representing a variable internal to the FSM.
 * @author xnovakf00
 * @date 07.05.2025
 */

#pragma once

#include <string>

/**
 * @class InternalVar
 * @brief Represents an internal variable used within the FSM.
 * 
 * This class encapsulates metadata about an internal variable, including its name, type,
 * and initial value.
 */
class InternalVar {
private:
    /**
     * The name of the internal variable.
    */ 
    std::string name;

    /**
     * The data type of the variable (e.g., int, string, bool).
     */
    std::string type;

    /**
     * The initial value assigned to the variable.
     */
    std::string initialValue;

public:
    /**
     * @brief Sets the name of the internal variable.
     * @param name The name to assign.
     * @return The assigned name.
     */
    std::string setName(std::string name);

    /**
     * @brief Sets the data type of the internal variable.
     * @param type The data type to assign (e.g., "int", "string").
     * @return The assigned type.
     */
    std::string setType(std::string type);

    /**
     * @brief Sets the initial value of the internal variable.
     * @param initialValue The initial value to assign.
     * @return The assigned initial value.
     */
    std::string setInitialValue(std::string initialValue);

    /**
     * @brief Gets the name of the internal variable.
     * @return The variable's name.
     */
    std::string getName();

    /**
     * @brief Gets the data type of the internal variable.
     * @return The variable's type.
     */
    std::string getType();

    /**
     * @brief Gets the initial value of the internal variable.
     * @return The variable's initial value.
     */
    std::string getInitialValue();
};
