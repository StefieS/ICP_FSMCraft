#include <QtMath>
#include <QGraphicsTextItem>
#include "TransitionItem.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

TransitionItem::TransitionItem(QPointF start, QPointF end, QGraphicsItem* parent)
    : QGraphicsLineItem(parent) {
    QPen pen(Qt::black, 2);
    setPen(pen);
    setLine(QLineF(start, end));
    setZValue(-1);

    label = new QGraphicsTextItem(this);
    label->setDefaultTextColor(Qt::darkBlue);
    label->setZValue(1); // Draw above the line
}

void TransitionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    constexpr int CircleRadius = 40;  // 100 diameter / 2
    constexpr qreal arrowSize = 12.0;

    QLineF fullLine = line();
    if (fullLine.length() == 0)
        return;

    painter->setPen(pen());

    // Only shorten line and draw arrow if confirmed
    if (confirmed) {
        QLineF shortened = fullLine;
        shortened.setLength(shortened.length() - (CircleRadius + arrowSize));
        painter->drawLine(shortened);

        // Compute angle pointing *towards* destination
        double angle = std::atan2(fullLine.dy(), fullLine.dx());

        QPointF arrowP1 = shortened.p2() - QPointF(std::cos(angle - M_PI / 6) * arrowSize,
                                                   std::sin(angle - M_PI / 6) * arrowSize);
        QPointF arrowP2 = shortened.p2() - QPointF(std::cos(angle + M_PI / 6) * arrowSize,
                                                   std::sin(angle + M_PI / 6) * arrowSize);

        QPolygonF arrowHead;
        arrowHead << shortened.p2() << arrowP1 << arrowP2;
        painter->setBrush(Qt::black);
        painter->drawPolygon(arrowHead);
    } else {
        // Still connecting — draw full line without arrow
        painter->drawLine(fullLine);
    }
}

void TransitionItem::setLabel(const QString& text) {
    label->setPlainText(text);
    updateLabelPosition();
}

void TransitionItem::markConfirmed() {
    confirmed = true;
    update(); // Triggers repaint
}

void TransitionItem::updateLabelPosition() {
    QLineF line = this->line();
    QPointF center = (line.p1() + line.p2()) / 2.0;

    // Compute angle in radians
    qreal angle = std::atan2(line.dy(), line.dx());

    // More vertical offset if line is horizontal
    qreal verticalOffset = 30.0 * std::cos(angle); // 30px when horizontal, ~0 when vertical

    // Offset perpendicular to the line
    QPointF offset(-line.dy(), line.dx());
    qreal length = std::hypot(offset.x(), offset.y());
    if (length > 0.0)
        offset /= length;

    QPointF finalPos = center + offset * verticalOffset;

    label->setPos(finalPos - QPointF(label->boundingRect().width() / 2, label->boundingRect().height() / 2));
}

void TransitionItem::updateLine(QPointF start, QPointF end) {
    setLine(QLineF(start, end));
    updateLabelPosition(); // Keep label in sync
}
