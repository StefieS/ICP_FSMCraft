#pragma once

#include <QGraphicsItemGroup>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>


class StateItem : public QGraphicsItemGroup {
public:
    StateItem(const QPointF& position, const QString& name);
    void rename();
    void setInitial(bool initial);  
    bool isInitial() const;         
    bool containsScenePoint(const QPointF& pt) const;
    QString getName() const;
    void rename(const std::function<bool(const QString&)>& isNameTaken);


private:
    QGraphicsEllipseItem* circle;
    QGraphicsTextItem* label;
    bool initial = false;           
};
