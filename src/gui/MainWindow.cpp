#include "MainWindow.h"
#include "StateItem.h"
#include "../fsm/FSM.h"
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
#include <QIcon>
#include <QAction>
#include <QDebug>
#include "../io/JsonMaker.h"
#include <QFile>
#include <QJsonDocument>

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

    // Create Run button
    QPushButton *newRunButton = new QPushButton(this);
    newRunButton->setText("Run");
    newRunButton->setToolTip("Run FSM");
    layout->addWidget(newRunButton, 0, Qt::AlignLeft);  // align left to prevent stretching
    connect(newRunButton, &QToolButton::clicked, this, &MainWindow::onRunClicked);

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

void MainWindow::onRunClicked() {
    fsm = new FSM("fsm_gen");

    for (int i = 0; i < stateCount; ++i) {
        std::shared_ptr<State> statePtr(stateList[i]);
        fsm->addState(statePtr);

        // Add transitions stored in this state
        for (const Transition& t : statePtr->getTransitions()) {
            fsm->addTransition(std::make_shared<Transition>(t));
        }

        // Prevent double delete: hand ownership to FSM
        stateList[i] = nullptr;
    }

    // TODO: Add inputs, outputs, internals

    JsonMaker maker;
    QJsonObject json = maker.toJson(fsm);

    QJsonDocument doc(json);
    QString fileName = "generated_fsm.json";
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Failed to write FSM JSON.");
        return;
    }

    file.write(doc.toJson());
    file.close();

    QMessageBox::information(this, "FSM Saved", "FSM saved and Run triggered.");
}


bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched != view->viewport())
        return QMainWindow::eventFilter(watched, event);

    // Only mouse events from the viewport
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress) 
    {
        auto *me = static_cast<QMouseEvent*>(event);
        QPointF scenePos = view->mapToScene(me->pos());

        // ─── Hover debug ──────────────────────────────────────────────
        if (event->type() == QEvent::MouseMove) {
            QGraphicsItem* under = scene->itemAt(scenePos, QTransform());
            qDebug() << " raw under =" << under;

            StateItem* state = nullptr;
            if (auto *g = dynamic_cast<StateItem*>(under)) {
                state = g;
            } else if (under && under->parentItem()) {
                state = dynamic_cast<StateItem*>(under->parentItem());
            }                  

            if (state) {
                QPointF center = state->scenePos();
                qDebug() << " Found StateItem at center=" << center
                         << " cursor=" << scenePos;
            } else {
                qDebug() << " Hovering over empty at" << scenePos;
            }
        }

        if (event->type() == QEvent::MouseMove && transitionStart && currentLine) {
            auto *me = static_cast<QMouseEvent*>(event);
            QPointF scenePos = view->mapToScene(me->pos());
            currentLine->updateLine(transitionStart->sceneCenter(), scenePos);
            return true;
        }

        if (event->type() == QEvent::MouseButtonRelease && me->button() == Qt::LeftButton && transitionStart) {
            QGraphicsItem* released = scene->itemAt(scenePos, QTransform());
            auto* target = dynamic_cast<StateItem*>(released ? released : released->parentItem());
        
            if (target) {
                // Draw final connection
                currentLine->updateLine(transitionStart->sceneCenter(), target->sceneCenter());
        
                // Add logical transition
                std::string src = transitionStart->getName().toStdString();
                std::string dst = target->getName().toStdString();
                auto* t = new Transition(src, dst, "in", "1");  // Adjust logic later
            } else {
                scene->removeItem(currentLine);  // Remove if no valid end
                delete currentLine;
            }
        
            transitionStart = nullptr;
            currentLine = nullptr;
            return true;
        }

        QGraphicsItem* under = scene->itemAt(scenePos, QTransform());
        auto* clicked = dynamic_cast<StateItem*>(under ? under : under->parentItem());
        if (clicked) {
            transitionStart = clicked;
            currentLine = new TransitionItem(clicked->sceneCenter(), scenePos);
            scene->addItem(currentLine);
            return true;
        }

        // ─── Mouse‐button‐press handling ──────────────────────────────
        if (me->button() == Qt::RightButton && event->type() == QEvent::MouseButtonPress) {
            QGraphicsItem* under = scene->itemAt(scenePos, QTransform());
            qDebug() << " Right-click raw item =" << under;

            StateItem* state = nullptr;
            if (auto *g = dynamic_cast<StateItem*>(under)) {
                state = g;
            } else if (under && under->parentItem()) {
                state = dynamic_cast<StateItem*>(under->parentItem());
            }                   

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
                            stateList[i] = nullptr;

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
