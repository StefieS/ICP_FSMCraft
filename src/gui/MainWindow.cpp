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
#include <QTimer>
#include <QAction>
#include <QDebug>
#include <QTextEdit>
#include "../io/JsonMaker.h"
#include <QFile>
#include <QJsonDocument>
#include <QRegularExpression>
#include "TransitionItem.h"
#include "QFlowLayout.h"
#include <QFileDialog>
#include <QDir>
#include "../io/JsonLoader.h"
#include "../messages/Message.h"
#include "../controllers/fsmController/FsmController.h"
#include "../controllers/guiController/GuiController.h"

constexpr int CircleDiameter = 80; 
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
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // Left column: toolbar, inputs, logs, etc.
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(4);

    // Right column: FSM editor (graphics view)
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(6);

    mainLayout->addLayout(leftLayout, 3);   // 3/12
    mainLayout->addLayout(rightLayout, 9);  // 9/12

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
    runButton = new QToolButton(toolbarWidget);
    runButton->setText("▶");
    runButton->setToolTip("Run FSM");
    runButton->setFixedSize(32, 32);
    toolbarLayout->addWidget(runButton);

    // New State Button
    QToolButton *newStateButton = new QToolButton(toolbarWidget);
    newStateButton->setText("◯");
    newStateButton->setToolTip("New state");
    newStateButton->setFixedSize(32, 32);
    toolbarLayout->addWidget(newStateButton);
    
    // Clear Button
    QToolButton *clearButton = new QToolButton(toolbarWidget);
    clearButton->setText("Clear"); 
    clearButton->setToolTip("Clear canvas");
    clearButton->setFixedSize(50, 32);
    toolbarLayout->addWidget(clearButton);
    
    // Save Button
    QToolButton *saveButton = new QToolButton(toolbarWidget);
    saveButton->setText("Save");
    saveButton->setToolTip("Save FSM");
    saveButton->setFixedSize(50, 32);
    toolbarLayout->addWidget(saveButton);

    // Upload Button
    QToolButton *uploadButton = new QToolButton(toolbarWidget);
    uploadButton->setText("Upload");
    uploadButton->setToolTip("Upload FSM");
    uploadButton->setFixedSize(50, 32);
    toolbarLayout->addWidget(uploadButton);

    QLineEdit* nameInputField = new QLineEdit(this);
    nameInputField->setPlaceholderText("Name");
    nameInputField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QPushButton* submitButton = new QPushButton("+", this);
    submitButton->setFixedSize(30, 30);
    toolbarLayout->addWidget(nameInputField);
    toolbarLayout->addWidget(submitButton);

    connect(submitButton, &QPushButton::clicked, this, [this, nameInputField]() {
        QString name = nameInputField->text();
        qDebug() << "Entered name:" << name;
        this->automatonName = name;
    });

    connect(runButton, &QToolButton::clicked, this, &MainWindow::onRunClicked);
    connect(newStateButton, &QToolButton::clicked, this, &MainWindow::onNewStateButtonClicked);
    connect(clearButton, &QToolButton::clicked, this, &MainWindow::onClearClicked);
    connect(saveButton, &QToolButton::clicked, this, &MainWindow::onSaveClicked);
    connect(uploadButton, &QToolButton::clicked, this, &MainWindow::onUploadClicked);

    leftLayout->addWidget(toolbarWidget, 0, Qt::AlignLeft);

    // Add env variable input container --
    QWidget* envInputRow = new QWidget(this);
    QGridLayout* envLayout = new QGridLayout(envInputRow);
    envLayout->setContentsMargins(0, 0, 0, 0);
    envLayout->setSpacing(6);

    QLineEdit* envKeyEdit = new QLineEdit(this);
    QLineEdit* envValueEdit = new QLineEdit(this);
    envKeyEdit->setPlaceholderText("Key");
    envValueEdit->setPlaceholderText("Value");
    envKeyEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    envValueEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QLabel* inputLabel1 = new QLabel("Declare internals:");
    inputLabel1->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(inputLabel1);
    
    QLabel* colonLabel1 = new QLabel(":", this);

    // Add env addition button
    QPushButton* addEnvButton = new QPushButton("Add", this);
    addEnvButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // Column stretch factors (total 12)
    envLayout->setColumnStretch(0, 4);  // key
    envLayout->setColumnStretch(1, 0);  // colon
    envLayout->setColumnStretch(2, 4);  // value
    envLayout->setColumnStretch(3, 4);  // button

    // Add widgets in row 0
    envLayout->addWidget(envKeyEdit,     0, 0);
    envLayout->addWidget(colonLabel1, 0, 1, Qt::AlignCenter);
    envLayout->addWidget(envValueEdit,   0, 2);
    envLayout->addWidget(addEnvButton,   0, 3);

    leftLayout->addWidget(envInputRow);
    leftLayout->addSpacing(2);

    internalVarsContainer = new QWidget(this);
    internalVarsFlow = new QFlowLayout(internalVarsContainer);
    internalVarsFlow->setContentsMargins(5, 5, 5, 5);
    internalVarsFlow->setSpacing(8);
    
    leftLayout->addWidget(internalVarsContainer);

    // Input container --
    QLabel* inputLabel2 = new QLabel("Define inputs:");
    inputLabel2->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(inputLabel2);

    // Create input row with grid layout
    QWidget* inputRow = new QWidget(this);
    QGridLayout* inputGridLayout = new QGridLayout(inputRow);
    inputGridLayout->setContentsMargins(0, 0, 0, 0);
    inputGridLayout->setSpacing(6);

    QLineEdit* inputNameEdit = new QLineEdit(this);
    inputNameEdit->setPlaceholderText("Input name");
    inputNameEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QLabel* colonLabel2 = new QLabel(this);

    //QLineEdit* inputInitialValueEdit = new QLineEdit(this);
    //nputInitialValueEdit->setPlaceholderText("Initial value");
    //inputInitialValueEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QPushButton* addInputButton = new QPushButton("Add", this);
    addInputButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // Column stretch factors (total 12)
    inputGridLayout->setColumnStretch(0, 4);  // name
    inputGridLayout->setColumnStretch(1, 0);  // colon
    inputGridLayout->setColumnStretch(2, 4);  // value
    inputGridLayout->setColumnStretch(3, 4);  // button

    // Add widgets to grid row 0
    inputGridLayout->addWidget(inputNameEdit,  0, 0);
    inputGridLayout->addWidget(colonLabel2,    0, 1, Qt::AlignCenter);
    //inputGridLayout->addWidget(inputInitialValueEdit, 0, 2);
    inputGridLayout->addWidget(addInputButton, 0, 3);

    leftLayout->addWidget(inputRow);

    QScrollArea* inputScrollArea = new QScrollArea(this);
    inputScrollArea->setWidgetResizable(true);
    inputScrollArea->setFixedHeight(150); // Optional: set max height

    inputListContainer = new QWidget(this);
    inputListLayout = new QVBoxLayout(inputListContainer);
    inputListLayout->setContentsMargins(5, 5, 5, 5);
    inputListLayout->setSpacing(8);
    inputListLayout->addStretch();

    inputScrollArea->setWidget(inputListContainer);
    leftLayout->addWidget(inputScrollArea);
    leftLayout->addSpacing(2);

    // Outputs Container
    QLabel* outputLabel = new QLabel("Define outputs:");
    outputLabel->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(outputLabel);

    QWidget* outputRow = new QWidget(this);
    QGridLayout* outputGridLayout = new QGridLayout(outputRow);
    outputGridLayout->setContentsMargins(0, 0, 0, 0);
    outputGridLayout->setSpacing(6);

    QLineEdit* outputNameEdit = new QLineEdit(this);
    outputNameEdit->setPlaceholderText("Output name");
    outputNameEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QLabel* colonLabel3 = new QLabel(this);

    //QLineEdit* outputValueEdit = new QLineEdit(this);
    //outputValueEdit->setPlaceholderText("Initial value");
    //outputValueEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QPushButton* addOutputButton = new QPushButton("Add", this);
    addOutputButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    outputGridLayout->setColumnStretch(0, 4);
    outputGridLayout->setColumnStretch(1, 0);
    outputGridLayout->setColumnStretch(2, 4);
    outputGridLayout->setColumnStretch(3, 4);

    outputGridLayout->addWidget(outputNameEdit,  0, 0);
    outputGridLayout->addWidget(colonLabel3,     0, 1, Qt::AlignCenter);
    //outputGridLayout->addWidget(outputValueEdit, 0, 2);
    outputGridLayout->addWidget(addOutputButton, 0, 3);

    leftLayout->addWidget(outputRow);

    QScrollArea* outputScrollArea = new QScrollArea(this);
    outputScrollArea->setWidgetResizable(true);
    outputScrollArea->setFixedHeight(150);

    outputListContainer = new QWidget(this);
    outputListLayout = new QVBoxLayout(outputListContainer);
    outputListLayout->setContentsMargins(5, 5, 5, 5);
    outputListLayout->setSpacing(8);
    outputListLayout->addStretch();

    outputScrollArea->setWidget(outputListContainer);
    leftLayout->addWidget(outputScrollArea);

    // Create scene and view
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QColor("#ffffff"));

    view = new QGraphicsView(scene, this);
    rightLayout->addWidget(view);

    view->setMouseTracking(true);
    view->viewport()->installEventFilter(this);

    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(addEnvButton, &QPushButton::clicked, this, [=]() {
        QString key = envKeyEdit->text().trimmed();
        QString val = envValueEdit->text().trimmed();

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

        envKeyEdit->clear();
        envValueEdit->clear();
    });

    connect(addInputButton, &QPushButton::clicked, this, [=]() {
        QString name = inputNameEdit->text().trimmed();
        //QString val = inputInitialValueEdit->text().trimmed();

        if (name.isEmpty()) return;

        if (inputMap.contains(name)) {
            QMessageBox::warning(this, "Duplicate Input", "Input already exists.");
            return;
        }

        QLineEdit* valField = new QLineEdit("?", this);
        valField->setReadOnly(true);
        valField->setFixedWidth(120);
        valField->setStyleSheet("QLineEdit { border: none; background: transparent; }");
        
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* label = new QLabel(name);
        QPushButton* removeButton = new QPushButton("✕");
        removeButton->setFixedSize(20, 20);

        row->addWidget(label);
        row->addWidget(valField);
        row->addWidget(removeButton);
        inputListLayout->insertLayout(inputListLayout->count() - 1, row);

        inputMap[name] = valField;

        connect(removeButton, &QPushButton::clicked, this, [=]() {
            label->deleteLater();
            valField->deleteLater();
            removeButton->deleteLater();
            row->deleteLater();
            inputMap.remove(name);
        });

        inputNameEdit->clear();
        //inputInitialValueEdit->clear();
    });
    
    connect(addOutputButton, &QPushButton::clicked, this, [=]() {
        QString name = outputNameEdit->text().trimmed();
        //QString val = outputValueEdit->text().trimmed();

        if (name.isEmpty()) return;

        if (outputMap.contains(name)) {
            QMessageBox::warning(this, "Duplicate Output", "Output already exists.");
            return;
        }

        QLineEdit* valField = new QLineEdit("?", this);
        valField->setStyleSheet("QLineEdit { border: none; background: transparent; }");
        valField->setFixedWidth(120);
        valField->setReadOnly(true);

        QHBoxLayout* row = new QHBoxLayout();
        QLabel* label = new QLabel(name);
        QPushButton* removeButton = new QPushButton("✕");
        removeButton->setFixedSize(20, 20);

        row->addWidget(label);
        row->addWidget(valField);
        row->addWidget(removeButton);
        outputListLayout->insertLayout(outputListLayout->count() - 1, row);

        outputMap[name] = valField;

        connect(removeButton, &QPushButton::clicked, this, [=]() {
            label->deleteLater();
            valField->deleteLater();
            removeButton->deleteLater();
            row->deleteLater();
            outputMap.remove(name);
        });

        outputNameEdit->clear();
        //outputValueEdit->clear();
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

    rightLayout->addWidget(logBox);

    QLabel* injectLabel = new QLabel("Inject input:");
    injectLabel->setContentsMargins(0, 2, 0, 0);  // "Inject input"
    leftLayout->addWidget(injectLabel);

    QWidget* injectRow = new QWidget(this);
    QGridLayout* injectLayout = new QGridLayout(injectRow);
    injectLayout->setContentsMargins(0, 0, 0, 0);
    injectLayout->setSpacing(6);

    inputComboBox = new QComboBox(this);
    inputValueEdit = new QLineEdit(this);
    injectInputButton = new QPushButton("Inject", this);
    QLabel* injectColon = new QLabel(":", this);

    // Stretch setup: 4 + 0 + 4 + 4 = 12
    injectLayout->setColumnStretch(0, 4);
    injectLayout->setColumnStretch(1, 0);
    injectLayout->setColumnStretch(2, 4);
    injectLayout->setColumnStretch(3, 4);

    injectLayout->addWidget(inputComboBox,     0, 0);
    injectLayout->addWidget(injectColon,       0, 1, Qt::AlignCenter);
    injectLayout->addWidget(inputValueEdit,    0, 2);
    injectLayout->addWidget(injectInputButton, 0, 3);

    leftLayout->addWidget(injectRow);
    leftLayout->addSpacing(2);
    connect(injectInputButton, &QPushButton::clicked, this, &MainWindow::onInjectInputClicked);


    this->connected = networkHandler.connectToServer();
    runButton->setEnabled(!connected); // only allow run if disconnected
    if (this->connected) {
        Message msg = Message();
        msg.buildRequestMessage();
        this->networkHandler.sendToHost(msg.toMessageString());
    }
}

void MainWindow::printLog(std::string logMessage) {
    QString msg = QString::fromStdString(logMessage);
    qDebug() << msg << "log is this";
    logBox->appendPlainText(msg);
}

void MainWindow::setRunning() {
    safePrint("I am in running");
}

void MainWindow::highlightItem(bool on, IActivable& item) {
    if (this->lastActive != nullptr)
        this->lastActive->setActive(false);
    item.setActive(on);
    this->lastActive = &item;
}

void MainWindow::showError(std::string errorMessage) {
    QString msg = QString::fromStdString(errorMessage);
    QMessageBox::critical(this, "Error", msg);
}

void MainWindow::showOutput(std::string outputID, std::string outputValue) {
    QString id = QString::fromStdString(outputID);
    QString val = QString::fromStdString(outputValue);
    if (outputMap.contains(id)) {
        outputMap[id]->setText(val);
    }
}

void MainWindow::showInput(std::string inputID, std::string inputValue) {
    QString id = QString::fromStdString(inputID);
    QString val = QString::fromStdString(inputValue);
    if (inputFieldMap.contains(id)) {
        inputFieldMap[id]->setText(val);
    }
}

void MainWindow::setInterfaceLocked(bool locked) {
    runButton->setEnabled(true);  // Always allow run/pause toggle
    injectInputButton->setEnabled(true);  // Always allow injection
    inputComboBox->setEnabled(true);      // ← Keep enabled
    inputValueEdit->setEnabled(true);     // ← Keep enabled

    // Disable other buttons except Run and Inject
    for (auto* btn : this->findChildren<QPushButton*>()) {
        if (btn != injectInputButton)
            btn->setEnabled(!locked);
    }

    for (auto* tool : this->findChildren<QToolButton*>()) {
        if (tool != runButton)
            tool->setEnabled(!locked);
    }

    // Disable all other edits EXCEPT injectValueEdit
    for (auto edit : this->findChildren<QLineEdit*>()) {
        if (edit != inputValueEdit)
            edit->setEnabled(!locked);
    }

    for (auto box : this->findChildren<QComboBox*>()) {
        if (box != inputComboBox)
            box->setEnabled(!locked);
    }

    view->setInteractive(!locked);
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
    if (listenerThread.joinable()) {
            listenerThread.join();
        }
}

void MainWindow::onNewStateButtonClicked() {
    addingNewState = true;
    ghostCircle->setVisible(true);
}

void MainWindow::onStopClicked() {
    Message msg;
    msg.buildStopMessage();
    networkHandler.sendToHost(msg.toMessageString());
    setInterfaceLocked(true);
    QMessageBox::information(this, "FSM Stopped", "FSM Stopped.");
}


void MainWindow::onRunClicked() {
    if (!isRunning) {
            // Start FSM logic
            onSaveClicked();
            setInterfaceLocked(true);

            inputComboBox->clear();
            for (const auto& name : inputMap.keys()) {
                inputComboBox->addItem(name);
            }

            isRunning = true;
            runButton->setText("⏸");  // Pause icon
            runButton->setToolTip("Pause FSM");

            qDebug() << "FSM Started";

            if (!controller) {
                controller = new GuiController(this);
            }

            // Check if listener thread is already running, if not, create one
            if (!listenerRunning) {
                listenerRunning = true;
                listenerThread = std::thread([this]() {
                    this->networkHandler.listen(8080);
                });

                // Set up message receiving with a timer instead of sleep
                QTimer::singleShot(1000, this, &MainWindow::startReceivingMessages);
            }

            networkHandler.connectToServer();
            QTimer::singleShot(1000, this, &MainWindow::sendInitialMessage);

        } else {
            // Pause FSM logic
            isRunning = false;
            runButton->setText("▶");  // Play icon
            runButton->setToolTip("Run FSM");
            // Trigger stop logic to pause FSM
            onStopClicked();

            // Set the listener flag to false and join the thread
            listenerRunning = false;
            if (listenerThread.joinable()) {
                listenerThread.join();  // Wait for the listener thread to finish
            }
        }
    }
void MainWindow::startReceivingMessages() {
        std::thread([this]() {
            while (listenerRunning) {
                std::string buffer = this->networkHandler.recvFromHost();
                Message toProcess(buffer);
                if (toProcess.getType() == EMessageType::STOP) {
                    this->networkHandler.closeConnection();
                    listenerRunning = false;  // Stop listening
                    break;
                }
                controller->performAction(toProcess);
            }
        }).detach();
    }

void MainWindow::sendInitialMessage() {
        this->connected = networkHandler.connectToServer();
        Message msg;
        msg.buildJsonMessage("generated_fsm.json"); // TODO: Replace with appropriate name
        networkHandler.sendToHost(msg.toMessageString());
    }


void MainWindow::onSaveClicked() {
    fsm = new FSM(this->automatonName.toStdString());

    // Add internal variables
    for (auto it = internalVarMap.begin(); it != internalVarMap.end(); ++it) {
        QString key = it.key();
        QString value = it.value()->getValue();

        InternalVar var;
        var.setName(key.toStdString());
        std::string detected = detectTypeFromValue(value);
        var.setType(detected);
        qDebug()<<"INITIAL VAR SET FOR *****" ;
        auto str = var.setInitialValue(value.toStdString());
        qDebug()<< QString::fromStdString(str);
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
    QString fileName = this->automatonName + ".json";
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Failed to write FSM JSON.");
        return;
    }

    file.write(doc.toJson());
    file.close();

    QMessageBox::information(this, "FSM Saved", "FSM saved.");
}

void MainWindow::onUploadClicked() {
    QString examplesPath = QDir("..").filePath("examples");
    QDir dir(QDir(examplesPath).absolutePath());

    if (!dir.exists()) {
        QMessageBox::warning(this, "Missing Directory", "The ~/examples/ directory does not exist.");
        return;
    }

    QStringList jsonFiles = dir.entryList(QStringList() << "*.json", QDir::Files);
    if (jsonFiles.isEmpty()) {
        QMessageBox::information(this, "No Files", "No .json FSM files found in ~/examples/");
        return;
    }

    // Let the user choose one from the list
    bool ok = false;
    QString selectedFile = QInputDialog::getItem(
        this,
        "Select FSM File",
        "Available FSMs:",
        jsonFiles,
        0,
        false,
        &ok
    );

    if (!ok || selectedFile.isEmpty())
        return;
    loadFSMFromJson(selectedFile.toStdString());
    
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
    dialog.setStyleSheet("");
    dialog.setWindowTitle("New Transition");
    dialog.setFixedSize(300, 300);

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
    actionEdit->setPlaceholderText(
    R"(
    fsm.output("out", 1);
    fsm.output("out2", fsm.elapsed());)");
    

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

       
        // if (event->type() == QEvent::MouseMove) {
        //     QGraphicsItem* under = scene->itemAt(scenePos, QTransform());
        //     qDebug() << " raw under =" << under;

        //     StateItem* state = nullptr;
        //     if (auto *g = dynamic_cast<StateItem*>(under)) {
        //         state = g;
        //     } else if (under && under->parentItem()) {
        //         state = dynamic_cast<StateItem*>(under->parentItem());
        //     }                  

        //     if (state) {
        //         QPointF center = state->scenePos();
        //         qDebug() << " Found StateItem at center=" << center
        //                  << " cursor=" << scenePos;
        //     } else {
        //         qDebug() << " Hovering over empty at" << scenePos;
        //     }
        // }

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
    QString inputName = inputComboBox->currentText().trimmed();
    QString value = inputValueEdit->text().trimmed();

    if (inputName.isEmpty()) return;

    if (!inputMap.contains(inputName)) {
        QMessageBox::warning(this, "Injection Failed", "No input field found for: " + inputName);
        return;
    }

    QLineEdit* field = inputMap.value(inputName, nullptr);
    if (!field) {
        QMessageBox::critical(this, "Error", "Input mapping broken for: " + inputName);
        return;
    }

    qDebug() << "Injecting input:" << inputName << " = " << value;
    field->setText(value);  // Set injected value visibly

    Message msg;
    msg.buildInputMessage(inputName.toStdString(), value.toStdString());
    this->networkHandler.sendToHost(msg.toMessageString());  // or socket->sendMessage(msg);
}

IActivable& MainWindow::getActivableItem(EItemType type, std::string itemID) {
    QString qid = QString::fromStdString(itemID);
    
    if (type == EItemType::STATE) {
        for (QGraphicsItem* item : scene->items()) {
            if (auto* state = dynamic_cast<StateItem*>(item)) {
                if (state->getName() == qid) {
                    qDebug() <<state->getName() << "LABEL";
                    return *state;
                }
            }
        }
    } else if (type == EItemType::TRANSITION) {
        for (QGraphicsItem* item : scene->items()) {
            if (auto* transition = dynamic_cast<TransitionItem*>(item)) {
                qDebug() <<transition->labelText() << "LABEL";
                if (transition->labelText() == qid) { 
                    return *transition;
                }
            }
        }
    }
    
    throw std::runtime_error("Activable item not found: " + itemID);
}

void MainWindow::loadFSMFromJson(std::string pathToJson) {
    QString examplesPath = QDir("..").filePath("examples");
    QDir dir(QDir(examplesPath).absolutePath());
    QString filePath = dir.filePath(QString::fromStdString(pathToJson));

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Unable to open selected file.");
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(fileData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::critical(this, "Parse Error", "Invalid JSON format.");
        return;
    }

    JsonLoader loader;
    FSM* loadedFsm = loader.fromJson(doc);
    this->fsm = loadedFsm;
    if (!loadedFsm) {
        QMessageBox::critical(this, "Error", "Failed to load FSM from JSON.");
        return;
    }

    // Pass loadedFsm to your existing population logic
    onClearClicked();

    // Clear old internal variable widgets
    for (auto it = internalVarMap.begin(); it != internalVarMap.end(); ++it) {
        internalVarsFlow->removeWidget(it.value());
        it.value()->deleteLater();
    }
    internalVarMap.clear();

    // Populate internal variables
    for (const auto& var : loadedFsm->getInternalVars()) {
        QString key = QString::fromStdString(var.getName());
        QString val = QString::fromStdString(var.getInitialValue());

        qDebug() << "[DEBUG] InternalVar:" << key << "=" << val; 

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
    }

    // Populate inputs
    for (const auto& name : loadedFsm->getInputNames()) {
        QString qName = QString::fromStdString(name);
        QLineEdit* valField = new QLineEdit(this);
        valField->setFixedWidth(120);
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* label = new QLabel(qName + ":");
        QPushButton* removeButton = new QPushButton("✕");
        removeButton->setFixedSize(20, 20);

        row->addWidget(label);
        row->addWidget(valField);
        row->addWidget(removeButton);
        inputListLayout->insertLayout(inputListLayout->count() - 1, row);
        inputMap[qName] = valField;

        connect(removeButton, &QPushButton::clicked, this, [=]() {
            label->deleteLater();
            valField->deleteLater();
            removeButton->deleteLater();
            row->deleteLater();
            inputMap.remove(qName);
        });
    }

    inputComboBox->clear();
    for (const QString& inputName : inputMap.keys()) {
        inputComboBox->addItem(inputName);
    }

    // Populate states and transitions
    int index = 0;
    const int cols = 5;
    const int spacing = 180;
    const int stateSize = CircleDiameter; // assuming same as your circle size

    // Calculate starting point (center the grid)
    int totalCols = std::min(cols, static_cast<int>(loadedFsm->getStates().size()));
    int totalRows = (loadedFsm->getStates().size() + cols - 1) / cols;
    QPointF center = view->mapToScene(view->viewport()->rect().center());
    QPointF start = center - QPointF((totalCols - 1) * spacing / 2, (totalRows - 1) * spacing / 2);

    for (const auto& pair : loadedFsm->getStates()) {
        std::shared_ptr<State> state = pair.second;
        QString qName = QString::fromStdString(state->getName());

        int row = index / cols;
        int col = index % cols;
        QPointF pos = start + QPointF(col * spacing, row * spacing);

        StateItem* stateItem = new StateItem(pos, qName);
        stateItem->setInitial(state->isInitialState());
        stateItem->setFinal(state->isFinalState());
        scene->addItem(stateItem);

        stateList[stateCount++] = new State(*state);
        ++index;
    }


    for (const auto& t : loadedFsm->getTransitions()) {
        QString from = QString::fromStdString(t->getSource());
        QString to   = QString::fromStdString(t->getTarget());
        StateItem* sourceItem = nullptr;
        StateItem* targetItem = nullptr;

        for (QGraphicsItem* item : scene->items()) {
            auto* s = dynamic_cast<StateItem*>(item);
            if (s) {
                if (s->getName() == from) sourceItem = s;
                else if (s->getName() == to) targetItem = s;
            }
        }

        if (sourceItem && targetItem) {
            auto* line = new TransitionItem(sourceItem->sceneCenter(), targetItem->sceneCenter());
            line->setLabel(QString::fromStdString(t->getInputEvent()) +" / "+ QString::fromStdString(t->getGuardCondition()));
            line->markConfirmed();
            scene->addItem(line);

            for (int i = 0; i < stateCount; ++i) {
                if (stateList[i]->getName() == t->getSource()) {
                    stateList[i]->addTransition(*t);
                }
            }
        }
    }

    QMessageBox::information(this, "Upload Complete", "FSM loaded successfully.");
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