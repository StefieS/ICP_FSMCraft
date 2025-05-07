#pragma once

#include <QAbstractTransition>
#include <QJSEngine>
#include <QString>
#include <QEvent>

class JsConditionTransition : public QAbstractTransition {
public:
JsConditionTransition(QJSEngine* engine, const QString& condition, QState* parentState)
: QAbstractTransition(parentState), jsEngine(engine), jsCondition(condition) {}

protected:
    bool eventTest(QEvent*) override {
        QJSValue result = jsEngine->evaluate(jsCondition);
        return result.isBool() && result.toBool();
    }

    void onTransition(QEvent*) override {}

private:
    QJSEngine* jsEngine;
    QString jsCondition;
};
