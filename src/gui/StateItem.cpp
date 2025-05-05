#include "StateItem.h"
#include <QInputDialog>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QMessageBox>

constexpr int CircleDiameter = 100;
constexpr int CircleRadius = CircleDiameter / 2;

StateItem::StateItem(const QPointF& position, const QString& name)
    : initial(false) {  // 👈 Initialize initial
    circle = new QGraphicsEllipseItem(-CircleRadius, -CircleRadius,
                                      CircleDiameter, CircleDiameter, this);
    circle->setPen(QPen(Qt::black, 2));
    circle->setBrush(QBrush(Qt::NoBrush));

    label = new QGraphicsTextItem(name, this);
    label->setPos(-label->boundingRect().width() / 2,
                  -label->boundingRect().height() / 2);

    setPos(position);
}

void StateItem::rename() {
    bool ok;
    QString newName = QInputDialog::getText(nullptr, "Rename State", "Enter new state name (max 8 chars):", QLineEdit::Normal, "", &ok);

    if (ok && !newName.isEmpty()) {
        if (newName.length() > 8)
            newName = newName.left(8);
        label->setPlainText(newName);

        label->setPos(-label->boundingRect().width() / 2,
                      -label->boundingRect().height() / 2);
    }
}

void StateItem::setInitial(bool initialState) {
    initial = initialState;
    if (initial) {
        circle->setPen(QPen(Qt::blue, 3));  // Blue thick border
    } else {
        circle->setPen(QPen(Qt::black, 2)); // Normal black border
    }
}

bool StateItem::isInitial() const {
    return initial;
}

bool StateItem::containsScenePoint(const QPointF& pt) const {
    // map the scene mouse-point into the group’s local coords
    QPointF local = mapFromScene(pt);
    // use the ellipse’s contains() which tests its exact shape
    return circle->contains(local);
}

QString StateItem::getName() const {
    return label->toPlainText();
}

void StateItem::rename(const std::function<bool(const QString&)>& isNameTaken) {
    bool ok;
    QString newName = QInputDialog::getText(nullptr, "Rename State",
        "Enter new state name (max 8 chars):", QLineEdit::Normal, "", &ok);

    if (ok && !newName.isEmpty()) {
        if (newName.length() > 8)
            newName = newName.left(8);

        // Check if name is taken (case-sensitive or insensitive as needed)
        if (isNameTaken(newName)) {
            QMessageBox::warning(nullptr, "Rename Failed",
                "State with the name \"" + newName + "\" already exists.");
            return;
        }

        label->setPlainText(newName);
        label->setPos(-label->boundingRect().width() / 2,
                      -label->boundingRect().height() / 2);
    }
}