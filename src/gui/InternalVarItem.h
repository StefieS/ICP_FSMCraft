// InternalVarItem.h
#pragma once

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QLabel>

class InternalVarItem : public QFrame {
    Q_OBJECT

public:
    explicit InternalVarItem(const QString& name, const QString& value, QWidget* parent = nullptr);
    QString getName() const;
    QString getValue() const;

signals:
    void removeRequested(const QString& name);

private:
    QString varName;
    QLabel* label;
    QPushButton* removeButton;
};