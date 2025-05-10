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
#include <QRegularExpression>
#include "TransitionItem.h"
#include "QFlowLayout.h"
#include "../messages/Message.h"
#include "../controllers/fsmController/FsmController.h"

constexpr int CircleDiameter = 100; 
constexpr int CircleRadius = CircleDiameter / 2; 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), addingNewState(false), ghostCircle(nullptr), networkHandler("127.0.0.1", 8080) {

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

    // Create compact horizontal toolbar aligned top-left
    QWidget *toolbarWidget = new QWidget(this);
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbarWidget);
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    toolbarLayout->setSpacing(5);

    // Run Button
    QToolButton *runButton = new QToolButton(toolbarWidget);
    runButton->setText("▶");
    runButton->setToolTip("Run FSM");
    runButton->setFixedSize(32, 32);
    toolbarLayout->addWidget(runButton);

    // Save Button
    QToolButton *saveButton = new QToolButton(toolbarWidget);
    saveButton->setText("SAVE");
    saveButton->setToolTip("Save FSM");
    saveButton->setFixedSize(32, 32);
    toolbarLayout->addWidget(saveButton);

    // Stop Button
    QToolButton *stopButton = new QToolButton(toolbarWidget);
    stopButton->setText("⏹");
    stopButton->setToolTip("Stop FSM");
    stopButton->setFixedSize(32, 32);
    toolbarLayout->addWidget(stopButton);
    
    // New State Button
    QToolButton *newStateButton = new QToolButton(toolbarWidget);
    newStateButton->setText("◯");
    newStateButton->setToolTip("New state");
    newStateButton->setFixedSize(32, 32);
    toolbarLayout->addWidget(newStateButton);
    
    QToolButton *clearButton = new QToolButton(toolbarWidget);
    clearButton->setText("Clear"); 
    clearButton->setToolTip("Clear canvas");
    clearButton->setFixedSize(45, 32);
    toolbarLayout->addWidget(clearButton);

    connect(runButton, &QToolButton::clicked, this, &MainWindow::onRunClicked);
    connect(saveButton, &QToolButton::clicked, this, &MainWindow::onSaveClicked);
    connect(stopButton, &QToolButton::clicked, this, &MainWindow::onStopClicked);
    connect(newStateButton, &QToolButton::clicked, this, &MainWindow::onNewStateButtonClicked);
    connect(clearButton, &QToolButton::clicked, this, &MainWindow::onClearClicked);

    layout->addWidget(toolbarWidget, 0, Qt::AlignLeft);

    // Add environment variable input row under toolbar
    QWidget* envInputRow = new QWidget(this);

    QHBoxLayout* envLayout = new QHBoxLayout(envInputRow);
    envLayout->setContentsMargins(0, 0, 0, 0);
    envLayout->setSpacing(6);

    QLineEdit* keyEdit = new QLineEdit(this);
    keyEdit->setPlaceholderText("key");
    keyEdit->setFixedWidth(120);

    QLabel* colonLabel = new QLabel(":", this);

    QLineEdit* valueEdit = new QLineEdit(this);
    valueEdit->setPlaceholderText("value");
    valueEdit->setFixedWidth(120);

    QPushButton* addEnvButton = new QPushButton("Add", this);
    addEnvButton->setFixedWidth(50);
    keyEdit->setFixedHeight(32);
    valueEdit->setFixedHeight(32);
    addEnvButton->setFixedHeight(32);

    envLayout->addWidget(keyEdit);
    envLayout->addWidget(colonLabel);
    envLayout->addWidget(valueEdit);
    envLayout->addWidget(addEnvButton);
    envLayout->addStretch();  // Pushes row to left

    layout->addWidget(envInputRow, 0, Qt::AlignLeft);

    // Internal variable container
    internalVarsContainer = new QWidget(this);
    internalVarsFlow = new QFlowLayout(internalVarsContainer);
    internalVarsFlow->setContentsMargins(5, 5, 5, 5);
    internalVarsFlow->setSpacing(8);

    layout->addWidget(internalVarsContainer);

    // Create scene and view
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QColor("#ffffff"));

    view = new QGraphicsView(scene, this);
    layout->addWidget(view);

    view->setMouseTracking(true);
    view->viewport()->installEventFilter(this);

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(addEnvButton, &QPushButton::clicked, this, [=]() {
        QString key = keyEdit->text().trimmed();
        QString val = valueEdit->text().trimmed();

        if (key.isEmpty())
            return;

        if (internalVarMap.contains(key)) {
            QMessageBox::warning(this, "Duplicate Variable", "Variable with this name already exists.");
            return;
        }

        auto* item = new InternalVarItem(key, val, this);
        connect(item, &InternalVarItem::removeRequested, this, [=](const QString& keyToRemove) {
            auto it = internalVarMap.find(keyToRemove);
            if (it != internalVarMap.end()) {
                internalVarsFlow->removeWidget(it.value());
                it.value()->deleteLater();
                internalVarMap.erase(it);
            }
        });

        internalVarsFlow->addWidget(item);
        internalVarMap[key] = item;

        keyEdit->clear();
        valueEdit->clear();
    });

    scene->setSceneRect(0, 0, view->width(), view->height());

    // Create ghost circle but keep it hidden
    ghostCircle = scene->addEllipse(0, 0, CircleDiameter, CircleDiameter,
        QPen(Qt::black, 2), QBrush(QColor(0, 0, 0, 0)));
                                    
    ghostCircle->setOpacity(0.5);
    ghostCircle->setVisible(false);

    logBox = new QPlainTextEdit(this);
    logBox->setReadOnly(true);
    logBox->setStyleSheet("background-color: #f9f9f9; border: 1px solid #ccc;");
    logBox->setMaximumHeight(120);
    logBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    layout->addWidget(logBox);


    this->connected = networkHandler.connectToServer();
    runButton->setEnabled(!connected); // only allow run if disconnected
    if (this->connected) {
        // send request message TODO
    }


}

void MainWindow::printLog(std::string logMessage) {
    QString msg = QString::fromStdString(logMessage);
    logBox->appendPlainText(msg);
}

void MainWindow::highlightItem(bool on, IActivable& item) {
    return;
}

void MainWindow::setRunning() {
    return;
}
void showError(std::string errorMessage) {
    return;
}

void showOutput(std::string outputID, std::string outputValue) {
    return;
}

static std::string detectTypeFromValue(const QString& value) {
    bool okInt = false;
    bool okFloat = false;

    value.toInt(&okInt);
    value.toFloat(&okFloat);

    if (okInt) return "int";
    if (okFloat) return "float";
    if (value == "true" || value == "false") return "bool";

    return "string";
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
    onSaveClicked();
    this->listenerThread = std::thread([this]() {
        this->networkHandler.listen(8080);
    });
    std::this_thread::sleep_for(std::chrono::seconds(1));
    networkHandler.connectToServer();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Message msg;
    msg.buildJsonMessage("generated_fsm.json"); // TODO NAME
    networkHandler.sendToHost(msg.toMessageString());
}

void MainWindow::onStopClicked() {
    // todo send message of stop
    Message msg;
    msg.buildStopMessage();
    networkHandler.sendToHost(msg.toMessageString());
    // todo disconnect
    QMessageBox::information(this, "FSM Stopped", "FSM Stopped.");
}

void MainWindow::onSaveClicked() {
        fsm = new FSM("fsm_gen");

    // Add internal variables
    for (auto it = internalVarMap.begin(); it != internalVarMap.end(); ++it) {
        QString key = it.key();
        QString value = it.value()->getValue();

        InternalVar var;
        var.setName(key.toStdString());
        std::string detected = detectTypeFromValue(value);
        var.setType(detected);
        var.setInitialValue(value.toStdString());

        fsm->addInternalVar(var);
    }

    QSet<QString> inputSet;
    QSet<QString> outputSet;
    QRegularExpression outputRegex(R"(output\(\s*["'](\w+)["'])");

    for (int index = 0; index < stateCount; ++index) {
        std::shared_ptr<State> statePtr(stateList[index]);
        fsm->addState(statePtr);

        // Extract outputs from action code
        QString action = QString::fromStdString(statePtr->getActionCode());
        QRegularExpressionMatchIterator matchIt = outputRegex.globalMatch(action);
        while (matchIt.hasNext()) {
            QRegularExpressionMatch match = matchIt.next();
            if (match.hasMatch()) {
                outputSet.insert(match.captured(1));
            }
        }

        // Process transitions and collect input names
        for (const Transition& t : statePtr->getTransitions()) {
            fsm->addTransition(std::make_shared<Transition>(t));

            QString input = QString::fromStdString(t.getInputEvent());
            if (!input.isEmpty()) {
                inputSet.insert(input);
            }
        }

        // Transfer ownership
        stateList[index] = nullptr;
    }

    // Add all unique inputs and outputs
    for (const QString& input : inputSet) {
        fsm->addInputName(input.toStdString());
    }
    for (const QString& output : outputSet) {
        fsm->addOutputName(output.toStdString());
    }

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

    QMessageBox::information(this, "FSM Saved", "FSM saved.");
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
        
            StateItem *state = new StateItem(finalPos, name);
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
