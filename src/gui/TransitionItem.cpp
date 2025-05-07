#include "TransitionItem.h"

TransitionItem::TransitionItem(QPointF start, QPointF end, QGraphicsItem* parent)
    : QGraphicsLineItem(parent) {
    QPen pen(Qt::black, 2);
    setPen(pen);
    setLine(QLineF(start, end));
    setZValue(-1); // Render below states
}

void TransitionItem::updateLine(QPointF start, QPointF end) {
    setLine(QLineF(start, end));
}
