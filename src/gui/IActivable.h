/**
 * @file IActivable.h
 * @brief Header file for the IActivable interface
 * @author xnovakf00
 * @date 06.05.2025
 */

#pragma once

/**
 * @brief IActivable interface
 * Used for objects in GUI that can be activated or deactivated.
 */
class IActivable {
public:
    virtual ~IActivable() = default;
    
    /**
     * @brief Set the active state of the object, in GUI means to highlight it
     * or turn the highlight off.
     * @param active True to activate, false to deactivate.
     */
    virtual void setActive(bool active) = 0;

    /**
     * @brief Check if the object is active.
     * @return True if active, false otherwise.
     */
    virtual bool isActive() const = 0;
};
