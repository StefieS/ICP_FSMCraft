#pragma once
#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QPen>
#include "IActivable.h"

class TransitionItem : public QGraphicsPathItem, public IActivable {
public:
    TransitionItem(QPointF start, QPointF end, QGraphicsItem* parent = nullptr, int id = 9999);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void updateLine(QPointF start, QPointF end);
    void setLabel(const QString& text);
    void markConfirmed();
    void setActive(bool active) override;
    bool isActive() const override;
    QString labelText() const;
    int getId() const;
    
private:
    QGraphicsTextItem* label = nullptr;
    bool active = false;
    bool confirmed = false;
    QPointF start;
    QPointF end;
    void updateLabelPosition();
    int id;
};
