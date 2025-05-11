#pragma once

#include <QObject>
#include <QString>
#include "QTfsm.h"
#include <QElapsedTimer>
class QTfsm; // forward declar
class QTBuiltinHandler : public QObject {
    Q_OBJECT
public:
    explicit QTBuiltinHandler(QObject* parent = nullptr, QTfsm* fsm = nullptr);

public slots:
    void output(const QString& name, const QString& value);
    int elapsed();
    bool defined(const QString& name);
    void stateEntered(QState* newState);
private:
    QTfsm* fsm;
    QElapsedTimer stateTimer;
    QState* lastActiveState = nullptr;
};
