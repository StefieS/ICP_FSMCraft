#pragma once
#include "State.h"
#include <QMainWindow>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include "FSM.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onInjectInputClicked();
    void onNewStateButtonClicked();

private:
    // FSM engine
    FSM* fsm = nullptr;

    static const int MAX_STATES = 100;
    State* stateList[MAX_STATES];
    int stateCount = 0;

    // Buttons
    QPushButton* injectButton = nullptr;
    QToolButton* newStateButton = nullptr;

    // State label
    QLabel* stateLabel = nullptr;

    // Graphics scene and view
    QGraphicsScene* scene = nullptr;
    QGraphicsView* view = nullptr;

    // State creation logic
    bool addingNewState = false;
    QGraphicsEllipseItem* ghostCircle = nullptr;
    // Helpers
    void debugPrintStateList() const;
};
