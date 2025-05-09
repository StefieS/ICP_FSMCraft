#include <QApplication>
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
#include <QTextEdit>
#include "../io/JsonMaker.h"
#include <QFile>
#include <QJsonDocument>
#include "TransitionItem.h"


constexpr int CircleDiameter = 100; 
constexpr int CircleRadius = CircleDiameter / 2; 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), addingNewState(false), ghostCircle(nullptr) {

    // Initialize state list
    for (int i = 0; i < MAX_STATES; ++i) {
        stateList[i] = nullptr;
    }
    

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    setFixedSize(1420, 800);

    QFile styleFile("./resources/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(styleSheet);  // Applies globally
    }


    // Create horizontal layout for top buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // + Button
    QToolButton *newStateButton = new QToolButton(this);
    newStateButton->setText("+");
    newStateButton->setToolTip("New state");
    buttonLayout->addWidget(newStateButton);
    connect(newStateButton, &QToolButton::clicked, this, &MainWindow::onNewStateButtonClicked);

    // Run Button
    QPushButton *newRunButton = new QPushButton("Run", this);
    newRunButton->setToolTip("Run FSM");
    buttonLayout->addWidget(newRunButton);
    connect(newRunButton, &QPushButton::clicked, this, &MainWindow::onRunClicked);

    // Clear Button
    QPushButton *clearButton = new QPushButton("Clear", this);
    clearButton->setToolTip("Clear canvas");
    buttonLayout->addWidget(clearButton);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClearClicked);

    int buttonWidth = 80;
    int buttonHeight = 30;

    newStateButton->setFixedSize(buttonWidth, buttonHeight);
    newRunButton->setFixedSize(buttonWidth, buttonHeight);
    clearButton->setFixedSize(buttonWidth, buttonHeight);


    // Add button row to main layout
    layout->addLayout(buttonLayout);

    // Create scene and view
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QColor("#ffffff"));

    view = new QGraphicsView(scene, this);
    layout->addWidget(view);

    view->setMouseTracking(true);
    view->viewport()->installEventFilter(this);

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene->setSceneRect(0, 0, view->width(), view->height());

    // Create ghost circle but keep it hidden
    ghostCircle = scene->addEllipse(0, 0, CircleDiameter, CircleDiameter,
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

void MainWindow::onClearClicked() {
    scene->clear();  // Remove all visual items
    stateCount = 0;

    for (int i = 0; i < MAX_STATES; ++i) {
        stateList[i] = nullptr;
    }

    // Reset any in-progress transitions or ghost elements
    transitionStart = nullptr;
    currentLine = nullptr;
    addingNewState = false;
    connectingMode = false;

    // Add ghostCircle back if needed
    ghostCircle = scene->addEllipse(0, 0, CircleDiameter, CircleDiameter,
        QPen(Qt::black, 2), QBrush(Qt::NoBrush));
        
    ghostCircle->setOpacity(0.5);
    ghostCircle->setVisible(false);
}

std::tuple<QString, QString, QString> MainWindow::askForTransitionDetails() {
    QDialog dialog(this);
    dialog.setWindowTitle("New Transition");
    dialog.setFixedSize(300, 200);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QLabel* inputLabel = new QLabel("Input name:", &dialog);
    QLineEdit* inputEdit = new QLineEdit(&dialog);
    inputEdit->setPlaceholderText("e.g. buttonPressed");

    QLabel* timeoutLabel = new QLabel("Timeout in ms (optional):", &dialog);
    QLineEdit* timeoutEdit = new QLineEdit(&dialog);
    timeoutEdit->setPlaceholderText("e.g. 5000");

    QLabel* condLabel = new QLabel("Guard condition:", &dialog);
    QTextEdit* condEdit = new QTextEdit(&dialog);
    condEdit->setPlaceholderText("e.g. value > 5");

    QPushButton* okButton = new QPushButton("OK", &dialog);
    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);

    layout->addWidget(inputLabel);
    layout->addWidget(inputEdit);
    layout->addWidget(condLabel);
    layout->addWidget(condEdit);
    layout->addWidget(timeoutLabel);
    layout->addWidget(timeoutEdit);
    layout->addWidget(okButton);

    if (dialog.exec() == QDialog::Accepted) {
        return { inputEdit->text(), condEdit->toPlainText(), timeoutEdit->text() };
    }

    return { "", "" , ""};
}

std::pair<QString, QString> MainWindow::askForStateDetails() {
    QDialog dialog(this);
    dialog.setWindowTitle("New State");
    dialog.setFixedSize(350, 300);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // State name
    QLabel* nameLabel = new QLabel("State name:", &dialog);
    QLineEdit* nameEdit = new QLineEdit(&dialog);
    nameEdit->setPlaceholderText("e.g. IDLE");

    // Action block
    QLabel* actionLabel = new QLabel("State action (JavaScript-style code):", &dialog);
    QTextEdit* actionEdit = new QTextEdit(&dialog);
    actionEdit->setPlainText(R"(if (defined("set_to")) {
        timeout = atoi(valueof("set_to"));
    }
    output("out", 0);
    output("rt", 0);)");
    

    // Buttons
    QHBoxLayout* buttonRow = new QHBoxLayout();
    QPushButton* okButton = new QPushButton("OK", &dialog);
    QPushButton* cancelButton = new QPushButton("Cancel", &dialog);
    buttonRow->addStretch();
    buttonRow->addWidget(cancelButton);
    buttonRow->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    layout->addWidget(nameLabel);
    layout->addWidget(nameEdit);
    layout->addWidget(actionLabel);
    layout->addWidget(actionEdit);
    layout->addLayout(buttonRow);

    if (dialog.exec() == QDialog::Accepted) {
        return {
            nameEdit->text().trimmed(),
            actionEdit->toPlainText().trimmed()
        };
    }

    return { "", "" }; // Cancelled
}

QString MainWindow::askToEditAction(const QString& currentCode) {
    QDialog dialog(this);
    dialog.setWindowTitle("Edit State Action");
    dialog.setFixedSize(400, 300);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QLabel* label = new QLabel("Edit action code:", &dialog);
    QTextEdit* editor = new QTextEdit(&dialog);
    editor->setPlainText(currentCode);

    QHBoxLayout* buttonRow = new QHBoxLayout();
    QPushButton* okButton = new QPushButton("OK", &dialog);
    QPushButton* cancelButton = new QPushButton("Cancel", &dialog);
    buttonRow->addStretch();
    buttonRow->addWidget(cancelButton);
    buttonRow->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    layout->addWidget(label);
    layout->addWidget(editor);
    layout->addLayout(buttonRow);

    if (dialog.exec() == QDialog::Accepted) {
        return editor->toPlainText().trimmed();
    }

    return {}; // Empty = cancel
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

        if (event->type() == QEvent::MouseMove && connectingMode && transitionStart && currentLine) {
            QPointF scenePos = view->mapToScene(static_cast<QMouseEvent*>(event)->pos());
            currentLine->updateLine(transitionStart->sceneCenter(), scenePos);
            return true;
        }
        

        if (connectingMode && event->type() == QEvent::MouseButtonPress && me->button() == Qt::LeftButton) {
            QGraphicsItem* under = scene->itemAt(scenePos, QTransform());
            StateItem* target = nullptr;
            if (auto* g = dynamic_cast<StateItem*>(under)) {
                target = g;
            } else if (under && under->parentItem()) {
                target = dynamic_cast<StateItem*>(under->parentItem());
            }
        
            if (target && transitionStart && target != transitionStart) {
                currentLine->updateLine(transitionStart->sceneCenter(), target->sceneCenter());
        
                std::string src = transitionStart->getName().toStdString();
                std::string dst = target->getName().toStdString();
                auto [input, cond, timeout] = askForTransitionDetails();

                std::string inputToSet;
                if (input.isEmpty()) {
                    inputToSet = "";
                } else {
                    inputToSet = input.toStdString();
                }
                
                std::string condToSet;
                if (cond.isEmpty()) {
                    condToSet = "";
                } else {
                    condToSet = cond.toStdString();
                }

                std::string timeoutToSet;
                if (timeout.isEmpty()) {
                    timeoutToSet = "0";
                } else {
                    timeoutToSet = timeout.toStdString();
                }

                Transition t(src, dst, inputToSet, condToSet, timeoutToSet);

                for (int i = 0; i < stateCount; ++i) {
                    if (stateList[i]->getName() == src) {
                        stateList[i]->addTransition(t);
                        break;
                    }
                }

                QString labelText = input + " / " + cond;
                currentLine->setLabel(labelText);
                currentLine->markConfirmed();
        
            } else {
                // Not a valid target: remove line
                scene->removeItem(currentLine);
                delete currentLine;
            }
        
            transitionStart = nullptr;
            currentLine = nullptr;
            connectingMode = false;
            return true;
        }

        QGraphicsItem* under = scene->itemAt(scenePos, QTransform());
        StateItem* clicked = nullptr;
        if (auto* g = dynamic_cast<StateItem*>(under)) {
            clicked = g;
        } else if (under && under->parentItem()) {
            clicked = dynamic_cast<StateItem*>(under->parentItem());
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
                QAction* setFinal = menu.addAction(state->isFinal()   ? "Unset Final"   : "Set Final");
                QAction* connect = menu.addAction("Connect");
                QAction* editAct = menu.addAction("Edit Action");

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

                else if (act == setFinal) {
                    bool newStateFinal = !state->isFinal();
            
                    // Unset final on all other states
                    for (QGraphicsItem* item : scene->items()) {
                        auto* s = dynamic_cast<StateItem*>(item);
                        if (s && s != state && s->isFinal()) {
                            s->setFinal(false);
                        }
                    }
            
                    for (int i = 0; i < stateCount; ++i) {
                        stateList[i]->setFinal(false);
                    }
            
                    for (int i = 0; i < stateCount; ++i) {
                        if (stateList[i]->getName() == state->getName().toStdString()) {
                            stateList[i]->setFinal(newStateFinal);
                        }
                    }
            
                    state->setFinal(newStateFinal);
                }
                
                else if (act == connect) {
                    transitionStart = state;
                    currentLine = new TransitionItem(state->sceneCenter(), state->sceneCenter());
                    scene->addItem(currentLine);
                    connectingMode = true;
                    return true;
                }

                else if (act == editAct) {
                    // Lookup corresponding FSM state object
                    for (int i = 0; i < stateCount; ++i) {
                        if (stateList[i] && stateList[i]->getName() == state->getName().toStdString()) {
                            QString current = QString::fromStdString(stateList[i]->getActionCode());
                            QString updated = askToEditAction(current);
                            if (!updated.isEmpty()) {
                                stateList[i]->setActionCode(updated.toStdString());
                                qDebug() << "Updated actionCode for state" << QString::fromStdString(stateList[i]->getName());
                            }
                            break;
                        }
                    }
                }
                

                return true;
            }
        }

        else if (addingNewState && me->button() == Qt::LeftButton && event->type() == QEvent::MouseButtonPress) {
            QPointF finalPos = ghostCircle->pos() + QPointF(CircleRadius, CircleRadius);
        
            auto [name, actionCode] = askForStateDetails();
            if (name.isEmpty()) {
                addingNewState = false;
                ghostCircle->setVisible(false);
                return true;
            }
        
            if (name.length() > 8) name = name.left(8);
            std::string nameStr = name.toStdString();
        
            for (int i = 0; i < stateCount; ++i) {
                if (stateList[i]->getName() == nameStr) {
                    QMessageBox::warning(this, "Creation Failed",
                        "State with the name \"" + name + "\" already exists.");
                    return true;  // Duplicate, keep ghost
                }
            }
        
            if (stateCount >= MAX_STATES) {
                QMessageBox::warning(this, "Creation Failed", "Maximum number of states reached.");
                return true;
            }
        
            auto *state = new StateItem(finalPos, name);
            scene->addItem(state);
            qDebug() << "Created state at:" << finalPos;
        
            stateList[stateCount++] = new State(name.toStdString(), actionCode.toStdString());
        
            debugPrintStateList();
        
            addingNewState = false;
            ghostCircle->setVisible(false);
            return true;
        }
        
    }

    // Ghost‐circle follow when adding
    if (addingNewState && event->type() == QEvent::MouseMove) {
        auto *me = static_cast<QMouseEvent*>(event);
        QPointF scenePos = view->mapToScene(me->pos());
        ghostCircle->setPos(scenePos - QPointF(CircleRadius, CircleRadius));
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
