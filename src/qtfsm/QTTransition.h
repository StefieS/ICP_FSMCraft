/**
 * @file QTTransition.h
 * @brief Custom transition for FSM states using JavaScript condition evaluation.
 * 
 * @author xnovakf00
 * @author xlesigm00
 * @date 08.05.2025
 */

#pragma once

#include <QAbstractTransition>
#include <QJSEngine>
#include <QString>
#include <QEvent>
#include <QTimer>
#include <QDateTime>
#include "QTConditionEvent.h"
#include "../common/EItemType.h"
#include "../messages/Message.h"

// forward decl
class QTfsm;  

/**
 * @class JsConditionTransition
 * @brief A custom QAbstractTransition that evaluates JavaScript conditions and handles delayed transitions.
 */
class JsConditionTransition : public QAbstractTransition {
private:
    /**
     * @brief The key of the input expected to trigger this transition.
     */
    QString inputKey;

    /**
     * @brief JavaScript expression used to compute the transition delay in milliseconds.
     */
    QString delayExpression;

    /**
     * @brief Pointer to the delay timer object.
     */
    QTimer* delayTimer = nullptr;

    /**
     * @brief Indicates if the transition is ready after the delay timer ends.
     */
    bool ready = false;

    /**
     * @brief Pointer to the fsm instance associated with this transition.
     */
    QTfsm* automaton;

    /**
     * @brief JavaScript engine used to evaluate the condition expression.
     */
    QJSEngine* jsEngine;

    /**
     * @brief JavaScript condition string that determines whether the transition should fire.
     */
    QString jsCondition;

public:
    /**
     * @brief Constructs a JsConditionTransition object.
     * 
     * @param engine Pointer to the JavaScript engine.
     * @param condition JavaScript condition to evaluate.
     * @param expectedInputKey Input key that triggers the transition.
     * @param parentState Parent state of this transition.
     * @param delayExpr Delay expression in JavaScript to defer transition.
     * @param fsm Pointer to the FSM this transition belongs to.
     */
    JsConditionTransition(QJSEngine* engine,
                          const QString& condition,
                          const QString& expectedInputKey,
                          QState* parentState,
                          const QString& delayExpr,
                          QTfsm* fsm)
        : QAbstractTransition(parentState),
          jsEngine(engine),
          jsCondition(condition),
          inputKey(expectedInputKey),
          delayExpression(delayExpr),
          automaton(fsm) {}

    /**
     * @brief Starts the delay timer based on the evaluated delay expression.
     * @return True if the delay is zero and the transition is immediately ready, false otherwise.
     */
    bool startDelayTimer() {
        ready = false;
        QJSValue result = jsEngine->evaluate(delayExpression);
        if (result.isError()) {
            qWarning() << "Invalid delay expression:" << delayExpression << "->" << result.toString();
            return false;
        }

        int delayMs = result.toInt();
        if (delayMs <= 0) {
            ready = true;
            return true;
        }

        delayTimer = new QTimer(this);
        delayTimer->setSingleShot(true);
        QObject::connect(delayTimer, &QTimer::timeout, this, [this]() {
            ready = true;
            auto* triggerEvent = new JsConditionEvent(automaton->map, this->inputKey);
            this->automaton->postEvent(triggerEvent);
        });

        delayTimer->start(delayMs);
        return false;
    }

    /**
     * @brief Cancels and deletes the active delay timer if it was started
     */
    void cancelDelayTimer() {
        if (delayTimer) {
            delayTimer->stop();
            delayTimer->deleteLater();
            delayTimer = nullptr;
        }
        ready = false;
    }

protected:
    /**
     * @brief Tests whether an event should trigger this transition.
     * 
     * @param event The incoming event to evaluate.
     * @return True if the event matches the condition and delay is done, false otherwise
     */
    bool eventTest(QEvent* event) override {
        if (ready) {
            return ready;
        }

        if (event->type() != JsConditionEventType) {
            return false;
        }

        auto* jsEvent = static_cast<JsConditionEvent*>(event);
        if (jsEvent->inputKey != this->inputKey) {
            return false;
        }

        // empty condition, epsilon
        if (this->jsCondition.isEmpty()) {
            if (!ready && !delayTimer) {
                return startDelayTimer();
            }
            return true;
        }

        // Inject event variables into JS context
        QVariantMap vars = jsEvent->eventData;
        for (auto it = vars.constBegin(); it != vars.constEnd(); ++it) {
            jsEngine->globalObject().setProperty(it.key(), jsEngine->toScriptValue(it.value()));
        }

        QJSValue result = jsEngine->evaluate(jsCondition);
        if (result.isError()) {
            qDebug() << "Condition error:" << jsCondition << result.toString();
            return false;
        }

        bool pass = result.isBool() && result.toBool();
        if (!pass) {
            qDebug() << "Condition evaluated to false:" << jsCondition;
            return false;
        }

        if (!ready && !delayTimer) {
            startDelayTimer();
            return false;
        }

        return ready;
    }

    /**
     * @brief Called when the transition occurs. Logs the transition for sending to gui
     * @param event The event that caused the transition.
     */
    void onTransition(QEvent*) override {
        QDateTime now = QDateTime::currentDateTime();
        QString timeStr = now.toString("yyyy-MM-dd hh:mm:ss");
        std::string timeStamp = timeStr.toStdString();

        EItemType elementType = EItemType::TRANSITION;
        std::string currentElement = inputKey.toStdString() + " / " + jsCondition.toStdString();

        Message log;
        log.buildLogMessage(
            timeStamp,
            elementType,
            currentElement,
            automaton->getStringMap(automaton->getInputs()),
            automaton->getStringMap(automaton->getOutputs()),
            automaton->getStringMap(automaton->getVars())
        );

        automaton->getNetworkHandler().sendToHost(log.toMessageString());
    }
};
