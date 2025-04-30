#include "MainWindow.h"
#include "StateItem.h"
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QInputDialog>
#include <QGraphicsSceneMouseEvent>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QGraphicsView>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QMenu>
#include <QAction>
#include <QDebug>


constexpr int CircleDiameter = 100; 
constexpr int CircleRadius = CircleDiameter / 2; 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), addingNewState(false), ghostCircle(nullptr) {

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    setFixedSize(1920, 800);

    // Create New State button
    QToolButton *newStateButton = new QToolButton(this);
    newStateButton->setText("+");
    newStateButton->setToolTip("New state");
    layout->addWidget(newStateButton);

    connect(newStateButton, &QToolButton::clicked, this, &MainWindow::onNewStateButtonClicked);

    // Create scene and view
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    layout->addWidget(view);

    view->setMouseTracking(true);
    view->viewport()->installEventFilter(this);

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene->setSceneRect(0, 0, view->width(), view->height());

    // Create ghost circle but keep it hidden
    ghostCircle = scene->addEllipse(-CircleRadius, -CircleRadius, CircleDiameter, CircleDiameter,
                                    QPen(Qt::black, 2), QBrush(QColor(0, 0, 0, 0)));
    ghostCircle->setOpacity(0.5);
    ghostCircle->setVisible(false);
}

MainWindow::~MainWindow() {}

void MainWindow::onNewStateButtonClicked() {
    addingNewState = true;
    ghostCircle->setVisible(true);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched != view->viewport())
        return QMainWindow::eventFilter(watched, event);

    // Only mouse events from the viewport
    if (event->type() == QEvent::MouseMove ||
        event->type() == QEvent::MouseButtonPress) 
    {
        auto *me = static_cast<QMouseEvent*>(event);
        QPointF scenePos = view->mapToScene(me->pos());

        // ─── Hover debug ──────────────────────────────────────────────
        if (event->type() == QEvent::MouseMove) {
            QGraphicsItem* under = scene->itemAt(scenePos, QTransform());
            qDebug() << " raw under =" << under;

            StateItem* state = nullptr;
            if      (auto *g = dynamic_cast<StateItem*>(under))      state = g;
            else if (under && under->parentItem())                   state = dynamic_cast<StateItem*>(under->parentItem());

            if (state) {
                QPointF center = state->scenePos();
                qDebug() << " Found StateItem at center=" << center
                         << " cursor=" << scenePos;
            } else {
                qDebug() << " Hovering over empty at" << scenePos;
            }
            // continue on to press handling
        }

        // ─── Mouse‐button‐press handling ──────────────────────────────
        if (me->button() == Qt::RightButton && event->type() == QEvent::MouseButtonPress) {
            QGraphicsItem* under = scene->itemAt(scenePos, QTransform());
            qDebug() << " Right-click raw item =" << under;

            StateItem* state = nullptr;
            if      (auto *g = dynamic_cast<StateItem*>(under))      state = g;
            else if (under && under->parentItem())                   state = dynamic_cast<StateItem*>(under->parentItem());

            if (state) {
                QPointF center = state->scenePos();
                qDebug() << "  -> on state!  center =" << center
                         << " cursor =" << scenePos;

                QMenu menu;
                QAction* setInit = menu.addAction(state->isInitial() ? "Unset Initial" : "Set Initial");
                QAction* ren     = menu.addAction("Rename");
                QAction* del     = menu.addAction("Delete");
                QAction* act     = menu.exec(me->globalPos());
                if      (act == setInit)  state->setInitial(!state->isInitial());
                else if (act == ren)      state->rename();
                else if (act == del) {
                    scene->removeItem(state);
                    delete state;
                }     
                return true;
            }
        }
        else if (addingNewState && me->button() == Qt::LeftButton && event->type() == QEvent::MouseButtonPress) {
            // Place new state
            addingNewState = false;
            ghostCircle->setVisible(false);

            bool ok;
            QString name = QInputDialog::getText(
              this, "New State",
              "Enter state name (max 8 chars):",
              QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                if (name.length() > 8) name = name.left(8);
                auto *state = new StateItem(scenePos, name);
                scene->addItem(state);
                qDebug() << "Created state at:" << scenePos;
            }
            return true;
        }
    }

    // ─── Ghost‐circle follow when adding ───────────────────────────
    if (addingNewState && event->type() == QEvent::MouseMove) {
        auto *me = static_cast<QMouseEvent*>(event);
        QPointF scenePos = view->mapToScene(me->pos());
        ghostCircle->setPos(scenePos.x() - CircleRadius,
                            scenePos.y() - CircleRadius);
        return true;
    }

    return QMainWindow::eventFilter(watched, event);
}


void MainWindow::onInjectInputClicked() {
    // No behavior yet
}
