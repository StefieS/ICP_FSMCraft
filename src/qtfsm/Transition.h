#pragma once
#include <QAbstractTransition>
#include <QJSEngine>
#include <QString>
#include <QEvent>

class JsConditionTransition : public QAbstractTransition {
public:
    JsConditionTransition(QJSEngine* engine, const QString& jsCondition, QObject* parent = nullptr)
        : QAbstractTransition(parent), jsEngine(engine), condition(jsCondition) {}

protected:
    bool eventTest(QEvent*) override {
        QJSValue result = jsEngine->evaluate(condition);
        return result.toBool(); // Only transition if JS condition evaluates to true
    }

    void onTransition(QEvent*) override {
        // Optional: transition side effects or logging
    }

private:
    QJSEngine* jsEngine;
    QString condition;
};
