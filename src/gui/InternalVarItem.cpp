#include "InternalVarItem.h"
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

InternalVarItem::InternalVarItem(const QString& name, const QString& value, QWidget* parent)
    : QFrame(parent), varName(name) {

    // Layout setup
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 6, 10, 6);  // Padding inside the pill
    layout->setSpacing(12);                   // Spacing between elements

    // Key label
    keyLabel = new QLabel(name + ":", this);

    // Value field (read-only)
    valueEdit = new QLineEdit(value, this);
    valueEdit->setFixedWidth(40);
    valueEdit->setReadOnly(true);
    valueEdit->setStyleSheet("QLineEdit { background: transparent; border: none; }");

    // Remove button
    removeButton = new QPushButton("✕", this);
    removeButton->setFixedSize(20, 20);
    removeButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            font-weight: bold;
        }
        QPushButton:hover {
            color: red;
        }
    )");

    layout->addWidget(keyLabel);
    layout->addWidget(valueEdit);
    layout->addWidget(removeButton);

    // Visual style of the item "pill"
    setStyleSheet(R"(
        InternalVarItem {
            background-color: rgb(154, 154, 154);
            color: black;
            border-radius: 6px;
            margin-right: 8px;
        }
    )");

    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    connect(removeButton, &QPushButton::clicked, this, [this]() {
        emit removeRequested(varName);
    });
}

QString InternalVarItem::getName() const {
    return varName;
}

QString InternalVarItem::getValue() const {
    return valueEdit->text();
}
