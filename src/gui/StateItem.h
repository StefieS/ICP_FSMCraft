#pragma once

#include <QGraphicsObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>

class StateItem : public QGraphicsObject {
    Q_OBJECT

public:
    StateItem(const QPointF& position, const QString& name);
    void rename();
    void setInitial(bool initial);  
    bool isInitial() const;         
    bool containsScenePoint(const QPointF& pt) const;
    QString getName() const;
    void rename(const std::function<bool(const QString&)>& isNameTaken);
    QPointF sceneCenter() const;

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
};
