#include "QFlowLayout.h"
#include <QtWidgets>

QFlowLayout::QFlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing) {
    setContentsMargins(margin, margin, margin, margin);
}

QFlowLayout::~QFlowLayout() {
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

void QFlowLayout::addItem(QLayoutItem *item) {
    itemList.append(item);
}

int QFlowLayout::horizontalSpacing() const {
    if (m_hSpace >= 0)
        return m_hSpace;
    else
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int QFlowLayout::verticalSpacing() const {
    if (m_vSpace >= 0)
        return m_vSpace;
    else
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

int QFlowLayout::count() const {
    return itemList.size();
}

QLayoutItem *QFlowLayout::itemAt(int index) const {
    return itemList.value(index);
}

QLayoutItem *QFlowLayout::takeAt(int index) {
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    return nullptr;
}

Qt::Orientations QFlowLayout::expandingDirections() const {
    return 0;
}

bool QFlowLayout::hasHeightForWidth() const {
    return true;
}

int QFlowLayout::heightForWidth(int width) const {
    return doLayout(QRect(0, 0, width, 0), true);
}

QSize QFlowLayout::minimumSize() const {
    QSize size;
    for (const QLayoutItem *item : itemList)
        size = size.expandedTo(item->minimumSize());
    size += QSize(2 * margin(), 2 * margin());
    return size;
}

QSize QFlowLayout::sizeHint() const {
    return minimumSize();
}

void QFlowLayout::setGeometry(const QRect &rect) {
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

int QFlowLayout::doLayout(const QRect &rect, bool testOnly) const {
    int x = rect.x();
    int y = rect.y();
    int lineHeight = 0;

    for (QLayoutItem *item : itemList) {
        QWidget *wid = item->widget();
        int spaceX = horizontalSpacing();
        int spaceY = verticalSpacing();
        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > rect.right() && lineHeight > 0) {
            x = rect.x();
            y += lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y();
}

int QFlowLayout::smartSpacing(QStyle::PixelMetric pm) const {
    QObject *parent = this->parent();
    if (!parent) return -1;
    else if (parent->isWidgetType()) {
        QWidget *pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    }
    return static_cast<QLayout *>(parent)->spacing();
}
