#pragma once

#include <QGraphicsObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include "IActivable.h"

class StateItem : public QGraphicsObject, public IActivable {
    Q_OBJECT

public:
    StateItem(const QPointF& position, const QString& name);
    void setInitial(bool initial);  
    bool isInitial() const;         
    void setFinal(bool final);      
    bool isFinal() const;           
    bool containsScenePoint(const QPointF& pt) const;
    void updatePen();
    QString getName() const;
    QPointF sceneCenter() const;
    void setActive(bool active) override;
    bool isActive() const override;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    // Required for QGraphicsObject
    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

private:
    QGraphicsEllipseItem* circle;
    QGraphicsTextItem* label;
    bool initial = false;       
    bool final = false;
};
