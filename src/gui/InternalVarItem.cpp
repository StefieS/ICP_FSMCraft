#include "InternalVarItem.h"
#include <QHBoxLayout>
#include <QFrame>

InternalVarItem::InternalVarItem(const QString& name, const QString& value, QWidget* parent)
    : QFrame(parent), varName(name) {

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setSpacing(8);

    keyLabel = new QLabel(name + ":", this);

    valueEdit = new QLineEdit(this);
    valueEdit->setText(value);
    valueEdit->setFixedWidth(60);
    valueEdit->setReadOnly(true);

    removeButton = new QPushButton("✕", this);
    removeButton->setFixedSize(24, 24);

    layout->addWidget(keyLabel);
    layout->addWidget(valueEdit);
    layout->addWidget(removeButton);

    this->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    this->setStyleSheet("background-color: #f4f4f4; border-radius: 4px;");

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
