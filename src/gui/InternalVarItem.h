/**
 * @file InternalVarItem.h
 * @brief Header file for the InternalVarItem UI component used in the FSM editor.
 * @author xmarina00
 * @date 11.05.2025
 */

#pragma once

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

/**
 * @brief UI component representing a single internal variable declaration.
 * 
 * This widget includes the variable's name, an editable value field,
 * and a remove button. It is used in the FSM editor interface
 * to let users define or remove internal state variables.
 */
class InternalVarItem : public QFrame {
    Q_OBJECT

public:
    /**
     * @brief Constructor for the InternalVarItem widget.
     * 
     * @param name The variable name to be shown.
     * @param value The initial value assigned to the variable.
     * @param parent Optional parent widget (typically the main UI panel).
     */
    explicit InternalVarItem(const QString& name, const QString& value, QWidget* parent = nullptr);

    /**
     * @brief Retrieves the variable name.
     * @return Name of the internal variable.
     */
    QString getName() const;

    /**
     * @brief Retrieves the current value from the input field.
     * @return Value of the internal variable.
     */
    QString getValue() const;

signals:
    /**
     * @brief Signal emitted when the remove button is clicked.
     * @param name The name of the variable to be removed.
     */
    void removeRequested(const QString& name);

private:
    QString varName;              ///< Internal name of the variable
    QLabel* label;                ///< (Unused) generic label, can be omitted
    QPushButton* removeButton;    ///< Button to trigger removal from the list
    QLabel* keyLabel;             ///< Label showing the variable name
    QLineEdit* valueEdit;         ///< Editable text field for the variable's value
};
