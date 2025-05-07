#include "StateItem.h"
#include <QInputDialog>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QMessageBox>

constexpr int CircleDiameter = 100;
constexpr int CircleRadius = CircleDiameter / 2;

StateItem::StateItem(const QPointF& position, const QString& name) : initial(false) {
    circle = new QGraphicsEllipseItem(-CircleRadius, -CircleRadius,
                                      CircleDiameter, CircleDiameter, this);
    circle->setPen(QPen(Qt::black, 2));
    circle->setBrush(QBrush(Qt::NoBrush));
    circle->setBrush(QBrush(Qt::white));

    label = new QGraphicsTextItem(name, this);
    label->setPos(-label->boundingRect().width() / 2,
                  -label->boundingRect().height() / 2);

    setPos(position);
    setAcceptHoverEvents(true);    
}

void StateItem::updatePen() {
    QPen pen;
    if (initial && final) {
        pen = QPen(Qt::green, 3);  // Both -> Green
    } else if (initial) {
        pen = QPen(Qt::blue, 3);
    } else if (final) {
        pen = QPen(Qt::red, 3);
    } else {
        pen = QPen(Qt::black, 2);
    }
    circle->setPen(pen);
}


void StateItem::setInitial(bool initialState) {
    initial = initialState;
    updatePen();
}

void StateItem::setFinal(bool finalState) {
    final = finalState;
    updatePen();
}

bool StateItem::isInitial() const {
    return initial;
}


bool StateItem::isFinal() const {
    return final;
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

void StateItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    QPen p = circle->pen();
    p.setWidth(p.width() + 1);  // Just thicken, don’t change color
    circle->setPen(p);
}

void StateItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    updatePen();  // Restore correct color and width
}


QPointF StateItem::sceneCenter() const {
    return mapToScene(circle->rect().center());
}

QRectF StateItem::boundingRect() const {
    return circle->boundingRect();  // or a fixed size if needed
}

void StateItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    // No painting here since sub-items (circle, label) handle their own painting
    Q_UNUSED(painter);
}