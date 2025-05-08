#pragma once

#include <QAbstractTransition>
#include <QJSEngine>
#include <QString>
#include <QEvent>
#include "QTTransitionEvent.h"

class JsConditionTransition : public QAbstractTransition {
public:
JsConditionTransition(QJSEngine* engine, const QString& condition, QState* parentState)
: QAbstractTransition(parentState), jsEngine(engine), jsCondition(condition) {}

protected:
    bool eventTest(QEvent* event) override {
        if (event->type() != JsConditionEvent::EventType)
            return false;

        auto* jsEvent = static_cast<JsConditionEvent*>(event);

        // Inject variables into the JS engine
        for (auto it = jsEvent->eventData.begin(); it != jsEvent->eventData.end(); ++it) {
            jsEngine->globalObject().setProperty(it.key(), jsEngine->toScriptValue(it.value()));
        }

        QJSValue result = jsEngine->evaluate(jsCondition);
        if (result.isError()) {
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
