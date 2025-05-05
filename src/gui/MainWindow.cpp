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
#include <QMessageBox>
#include <QAction>
#include <QDebug>


constexpr int CircleDiameter = 100; 
constexpr int CircleRadius = CircleDiameter / 2; 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), addingNewState(false), ghostCircle(nullptr) {

    // Initialize state list
    for (int i = 0; i < MAX_STATES; ++i) {
        stateList[i] = nullptr;
    }


    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    setFixedSize(1420, 800);

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

MainWindow::~MainWindow() {
    for (int i = 0; i < stateCount; ++i) {
        delete stateList[i];
        stateList[i] = nullptr;
    }
}


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
                if (act == setInit) {
                    bool newStateInit = !state->isInitial();

                    // Reset all other StateItem visuals to non-initial
                    for (QGraphicsItem* item : scene->items()) {
                        auto* s = dynamic_cast<StateItem*>(item);
                        if (s && s != state && s->isInitial()) {
                            s->setInitial(false);
                        }
                    }

                
                    // Deselect the old initial
                    for (int i = 0; i < stateCount; ++i) {
                        if (stateList[i]->isInitialState()) {
                            stateList[i]->setInitial(false);
                        }
                    }
                
                    // Set this state as initial in the logical model
                    for (int i = 0; i < stateCount; ++i) {
                        if (stateList[i]->getName() == state->getName().toStdString()) {
                            stateList[i]->setInitial(newStateInit);
                        }
                    }

                
                    // Update the visual StateItem
                    state->setInitial(newStateInit);
                }
                
                else if (act == ren) {
                    QString oldName = state->getName();
                    state->rename([=](const QString& newName) {
                        if (newName == oldName) return false;
                        std::string newNameStr = newName.toStdString();
                        for (int i = 0; i < stateCount; ++i) {
                            if (stateList[i]->getName() == newNameStr) {
                                return true;
                            }
                        }
                        return false;
                    });
                
                    QString newName = state->getName();  // Now holds new name from rename()
                    for (int i = 0; i < stateCount; ++i) {
                        if (stateList[i]->getName() == oldName.toStdString()) {
                            stateList[i]->setName(newName.toStdString());
                            break;
                        }
                    }
                }

                else if (act == del) {
                    QString stateName = state->getName();
                    std::string stateNameStr = stateName.toStdString();
                
                    // Remove from logical state list
                    for (int i = 0; i < stateCount; ++i) {
                        if (stateList[i] && stateList[i]->getName() == stateNameStr) {
                            delete stateList[i];

                            // Shift remaining elements left
                            for (int j = i; j < stateCount - 1; ++j) {
                                stateList[j] = stateList[j + 1];
                            }

                            stateList[stateCount - 1] = nullptr;
                            stateCount--;
                            break;
                        }
                    }
                                    
                    // Remove from scene and delete visual
                    scene->removeItem(state);
                    delete state;
                
                    qDebug() << "Deleted state:" << stateName;
                    debugPrintStateList();
                }
                
                return true;
            }
        }
        else if (addingNewState && me->button() == Qt::LeftButton && event->type() == QEvent::MouseButtonPress) {
            // Place new state
            addingNewState = false;
            ghostCircle->setVisible(false);

            bool ok;
            QString name = QInputDialog::getText(this, "New State", "Enter state name (max 8 chars):", QLineEdit::Normal, "", &ok);
            if (ok && !name.isEmpty()) {
                if (name.length() > 8) name = name.left(8);
                std::string nameStr = name.toStdString();
            
                // Check for duplicates first
                bool exists = false;
                for (int i = 0; i < stateCount; ++i) {
                    if (stateList[i]->getName() == nameStr) {
                        exists = true;
                        break;
                    }
                }
            
                if (exists) {
                    QMessageBox::warning(this, "Creation Failed", 
                        "State with the name \"" + name + "\" already exists.");
                    return true;
                }
            
                if (stateCount >= MAX_STATES) {
                    QMessageBox::warning(this, "Creation Failed", "Maximum number of states reached.");
                    return true;
                }
            
                // ✅ Now safe to create and show
                auto *state = new StateItem(scenePos, name);
                scene->addItem(state);
                qDebug() << "Created state at:" << scenePos;
            
                stateList[stateCount++] = new State(nameStr, "");
                debugPrintStateList();
                qDebug() << "StateList now has" << stateCount << "items";
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
    // TODO
}

void MainWindow::debugPrintStateList() const {
    qDebug() << "==== DEBUG: stateList ====";
    for (int i = 0; i < stateCount; ++i) {
        const State* s = stateList[i];
        qDebug() << "State name:" << QString::fromStdString(s->getName())
                 << " | isInitial:" << s->isInitialState()
                 << " | actionCode:" << QString::fromStdString(s->getActionCode())
                 << " | transitions:" << s->getTransitions().size();
    }
    qDebug() << "===========================";
}
