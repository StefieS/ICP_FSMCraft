#pragma once

#include <QAbstractTransition>
#include <QJSEngine>
#include <QString>
#include <QEvent>
#include "QTConditionEvent.h"
#include <QTimer>
class JsConditionTransition : public QAbstractTransition {
private:
    QString inputKey;
    QString delayExpression;
    QTimer* delayTimer = nullptr;
    bool ready = false;
    QTfsm* automaton;
public:
    JsConditionTransition(QJSEngine* engine, 
        const QString& condition, 
        const QString& expectedInputKey, 
        QState* parentState,
        const QString& delayExpr,
        QTfsm* fsm): 
        QAbstractTransition(parentState),
        jsEngine(engine), 
        jsCondition(condition), 
        inputKey(expectedInputKey), 
        delayExpression(delayExpr),
        automaton(fsm){}

        bool startDelayTimer() {
            ready = false;
            
            // Evaluate using JS engine
            QJSValue result = jsEngine->evaluate(delayExpression);

            // cannot be converted to int, just ignore it
            if (result.isError() || !result.isNumber()) {
                qWarning() << "Invalid delay expression:" << delayExpression << "->" << result.toString();
                ready = false;
                return false;
            }

            int delayMs = result.toInt();

            // dont have to wait
            if (delayMs <= 0) {
                ready = true;
                return true;
            }
            qDebug() << "start for " << delayMs;
            delayTimer = new QTimer(this);
            delayTimer->setSingleShot(true);
            QObject::connect(delayTimer, &QTimer::timeout, this, [this]() {
                ready = true;
                QVariantMap map;
                auto* triggerEvent = new JsConditionEvent(map, this->inputKey); // TODO MAP THE GLOBAL ONE
                this->automaton->postEvent(triggerEvent);
            });

            delayTimer->start(delayMs);
            return false;
    }

    void cancelDelayTimer() {
        if (delayTimer) {
            delayTimer->stop();
            delayTimer->deleteLater();
            delayTimer = nullptr;
        }
        ready = false;
    }


protected:
    bool eventTest(QEvent* event) override {
        if (ready) { // in case of retrigger by timeout
            return ready;
        }

        if (event->type() != JsConditionEventType) {
            return false;
        }

        auto* jsEvent = static_cast<JsConditionEvent*>(event);

        // check if the expected input trigger
        if (jsEvent->inputKey != this->inputKey) {
            return false;
        }

        // epsilon
        if (this->jsCondition == "") {
            if (!ready && !delayTimer) { 
                return startDelayTimer(); // timer set    
            }

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
        
        // should trigger
        bool pass = result.isBool() && result.toBool();
        if (!pass) {
            return false;
        }

        // timeout
        if (!ready && !delayTimer) {
            startDelayTimer();  
            return false;       
        }

        return ready;

    }

    void onTransition(QEvent*) override {
        //TODO send Log
    }

private:
    QJSEngine* jsEngine;
    QString jsCondition;
};
