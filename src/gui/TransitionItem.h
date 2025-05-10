#pragma once
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QPen>
#include "IActivable.h"

class TransitionItem : public QGraphicsLineItem, public IActivable {
public:
    TransitionItem(QPointF start, QPointF end, QGraphicsItem* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void updateLine(QPointF start, QPointF end);
    void setLabel(const QString& text);
    void markConfirmed();
    void setActive(bool active) override;
    bool isActive() const override;
    QString labelText() const;
    
private:
    QGraphicsTextItem* label = nullptr;
    bool active = false;
    bool confirmed = false;
    void updateLabelPosition();
};
