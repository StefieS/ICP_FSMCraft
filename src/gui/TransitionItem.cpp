#include <QtMath>
#include <QGraphicsTextItem>
#include "TransitionItem.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

TransitionItem::TransitionItem(QPointF start, QPointF end, QGraphicsItem* parent, int id)
    : QGraphicsPathItem(parent), id(id), start(start), end(end) {
    QPen pen(Qt::black, 2);
    setPen(pen);
    setZValue(-1);

    label = new QGraphicsTextItem(this);
    label->setDefaultTextColor(Qt::darkBlue);
    label->setZValue(1); // Draw above the line

    updateLine(start, end);
}

int TransitionItem::getId() const {
    return this->id;
}

void TransitionItem::paint(QPainter* painter,
                            const QStyleOptionGraphicsItem* /*option*/,
                            QWidget* /*widget*/)
{
    // ---------- basic constants ----------
    constexpr qreal PEN_ACTIVE_W   = 3.0;
    constexpr qreal PEN_IDLE_W     = 2.0;
    constexpr qreal ARROW_LEN      = 12.0; 

    // ---------- draw the curve itself ----------
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(active ? Qt::red : Qt::black,
                         active ? PEN_ACTIVE_W : PEN_IDLE_W,
                         Qt::SolidLine,
                         Qt::RoundCap,
                         Qt::RoundJoin));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path());                 // the path was built in updateLine()

    // don’t add an arrow while the user is still dragging
    if (!confirmed)
        return;

    // ---------- decide where the arrow should sit ----------
    const bool selfLoop = (start == end);

    QPointF tip, back;

    if (selfLoop) {
        tip  = path().pointAtPercent(0.12);
        back = path().pointAtPercent(0.18);
    } else {
        /* Walk backwards from 1.0 until we are far enough from the
        target-state centre (end point). */
        constexpr qreal STATE_R   = 40.0;        // your state radius
        constexpr qreal GAP_AHEAD = STATE_R + ARROW_LEN + 2.0;   // +2 px safety

        qreal tTip = 1.0;
        for (qreal t = 1.0; t >= 0.0; t -= 0.01) {        // fine-grained search
            if (QLineF(path().pointAtPercent(t), end).length() >= GAP_AHEAD) {
                tTip = t;
                break;
            }
        }
        tip  = path().pointAtPercent(tTip);
        back = path().pointAtPercent(std::max(0.0, tTip - 0.03));   // ~3 % earlier    
        constexpr qreal SHIFT = 0.06;            // try 0.02 – 0.06 to taste
        const qreal tTipShift = std::min(0.99, tTip + SHIFT);

        tip  = path().pointAtPercent(tTipShift);
        back = path().pointAtPercent(std::max(0.0, tTipShift - 0.03));

    }


    // ---------- draw the curve (full – your code) ----------
    painter->drawPath(path());


    const QPointF dir   = tip - back;
    // angle of the tangent in the direction tip ➞ destination
    const qreal angle = std::atan2(dir.y(), dir.x());
    constexpr qreal ARROW_HALF_ANG = 20.0 * M_PI / 180.0;

    /* go BACK from the tip by ARROW_LEN along two directions
    rotated ±ARROW_HALF_ANG from the tangent                 */
    const QPointF p1 = tip - QPointF(std::cos(angle - ARROW_HALF_ANG) * ARROW_LEN,
                                    std::sin(angle - ARROW_HALF_ANG) * ARROW_LEN);
    const QPointF p2 = tip - QPointF(std::cos(angle + ARROW_HALF_ANG) * ARROW_LEN,
                                    std::sin(angle + ARROW_HALF_ANG) * ARROW_LEN);

    painter->setBrush(Qt::black);
    painter->drawPolygon(QPolygonF() << tip << p1 << p2);
}


void TransitionItem::setLabel(const QString& text) {
    label->setPlainText(text);
    updateLabelPosition();
}

void TransitionItem::markConfirmed() {
    confirmed = true;
    update();
}

void TransitionItem::updateLabelPosition() {
    QPointF center = path().pointAtPercent(0.5);
    QPointF direction = path().pointAtPercent(0.55) - path().pointAtPercent(0.45);
    QPointF normal(-direction.y(), direction.x());

    qreal length = std::hypot(normal.x(), normal.y());
    if (length > 0.0)
        normal /= length;

    QPointF finalPos = center + normal * 25.0;

    label->setPos(finalPos - QPointF(label->boundingRect().width() / 2, label->boundingRect().height() / 2));
}


// Radius of the state circle that MainWindow uses
static constexpr qreal STATE_R = 40.0;          // 80-pixel diameter in MainWindow
static constexpr qreal LOOP_GAP = 4.0;          // small gap between loop and state
static constexpr qreal LOOP_RADIUS = STATE_R + 12.0;   // how “wide” the loop is

void TransitionItem::updateLine(QPointF s, QPointF e)
{
    prepareGeometryChange();
    start = s;
    end   = e;

    QPainterPath p;

    // ───────────────────────────── normal edge ─────────────────────────────
    if (start != end) {
        QPointF centre = (start + end) / 2.0;
        QPointF dir    = end - start;
        QPointF normal(-dir.y(), dir.x());

        normal /= std::hypot(normal.x(), normal.y());          // unit normal
        QPointF ctrl = centre + normal * 40.0;                 // 40 → bulge

        p.moveTo(start);
        p.quadTo(ctrl, end);
    }

    // ───────────────────────────── self-loop ────────────────────────────────
    else
    {
        constexpr qreal GAP      = 1.0;          // 1-px breathing space
        constexpr qreal R_STATE  = STATE_R / 1.5;      // radius of the state
        constexpr qreal R_LOOP   = STATE_R;      // loop radius  = state radius

        /*  Geometry
        *  ---------
        *  State centre   : (start.x(), start.y())
        *  Loop centre    : exactly R_STATE + GAP above the state centre
        *
        *  We draw an arc that starts **at the top of the state** (-90°) and
        *  sweeps counter-clockwise  270°  so it comes back to meet the state
        *  at about -90°+270° = 180° (far left).  The arrow will later be put
        *  at 15 % of that arc (near the very top).
        */

        const QPointF loopCtr(start.x(),
                            start.y() - (R_STATE + GAP + R_LOOP));

        const QRectF box(loopCtr.x() - R_LOOP,
                        loopCtr.y() - R_LOOP,
                        R_LOOP * 2,
                        R_LOOP * 2);

        const qreal startDeg = -95.0;   // top of the circle
        const qreal sweepDeg = 360.0;   // CCW

        p.moveTo(loopCtr + QPointF(0,  R_LOOP));          // top point
        p.arcTo(box, startDeg, sweepDeg);                  // draw arc
    }




    setPath(p);
    updateLabelPosition();            // uses the curve now

    // force repaint, otherwise a stale bounding-rect may clip the new loop
    update();
}


void TransitionItem::setActive(bool isNowActive) {
    active = isNowActive;
    setPen(QPen(active ? Qt::red : Qt::black, active ? 3 : 2));
    update();
}

bool TransitionItem::isActive() const {
    return active;
}

QString TransitionItem::labelText() const {
    return label->toPlainText();
}
