#pragma once

#include <QObject>
#include <QString>

class QTBuiltinHandler : public QObject {
    Q_OBJECT
public:
    explicit QTBuiltinHandler(QObject* parent = nullptr);

public slots:
    void output(const QString& name, const QString& value);  // Slot declaration
};
