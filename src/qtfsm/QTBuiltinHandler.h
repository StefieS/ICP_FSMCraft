#pragma once

#include <QObject>
#include <QString>
#include "QTfsm.h"
class QTBuiltinHandler : public QObject {
    Q_OBJECT
public:
    explicit QTBuiltinHandler(QObject* parent = nullptr, QTfsm* fsm = nullptr);

public slots:
    void output(const QString& name, const QString& value);
    int elapsed();
private:
    QTfsm* fsm;
};
