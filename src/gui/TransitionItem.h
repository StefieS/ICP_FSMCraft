#pragma once
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QPen>

class TransitionItem : public QGraphicsLineItem {
public:
    TransitionItem(QPointF start, QPointF end, QGraphicsItem* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void updateLine(QPointF start, QPointF end);
    void setLabel(const QString& text);
    void markConfirmed();

private:
    QGraphicsTextItem* label = nullptr;
    bool confirmed = false;
    void updateLabelPosition();
};
