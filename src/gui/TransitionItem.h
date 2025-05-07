#pragma once
#include <QGraphicsLineItem>
#include <QPen>

class TransitionItem : public QGraphicsLineItem {
public:
    TransitionItem(QPointF start, QPointF end, QGraphicsItem* parent = nullptr);

    void updateLine(QPointF start, QPointF end);
};
