#include <QGraphicsSceneHoverEvent>

class StateItem : public QGraphicsItemGroup {
    Q_OBJECT  // needed if you use signals/slots (optional here)

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

private:
    QGraphicsEllipseItem* circle;
    QGraphicsTextItem* label;
    bool initial = false;       
};
