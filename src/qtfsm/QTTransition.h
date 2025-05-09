#pragma once

#include <QAbstractTransition>
#include <QJSEngine>
#include <QString>
#include <QEvent>
#include "QTConditionEvent.h"

class JsConditionTransition : public QAbstractTransition {
private:
    QString inputKey;
public:
JsConditionTransition(QJSEngine* engine, const QString& condition, const QString& expectedInputKey, QState* parentState)
: QAbstractTransition(parentState), jsEngine(engine), jsCondition(condition), inputKey(expectedInputKey){}

protected:
    bool eventTest(QEvent* event) override {
        if (event->type() != JsConditionEventType) {
            return false;
        }

        auto* jsEvent = static_cast<JsConditionEvent*>(event);

        // check if the expected input was added
        if (jsEvent->inputKey != this->inputKey) {
            return false;
        }

        if (this->jsCondition == "") { // epsilon
            return true;
        }

        // Inject variables into the JS engine
        QVariantMap vars = jsEvent->eventData;
        for (auto it = vars.constBegin(); it != vars.constEnd(); ++it) {
            jsEngine->globalObject().setProperty(it.key(), jsEngine->toScriptValue(it.value()));
        }

        QJSValue result = jsEngine->evaluate(jsCondition);
        if (result.isError()) {
            qWarning() <<jsCondition;
            qWarning() << "JavaScript error:" << result.toString();
            return false;
        }

        return result.isBool() && result.toBool();
    }
    void onTransition(QEvent*) override {}

private:
    QJSEngine* jsEngine;
    QString jsCondition;
};
