#pragma once
#include <QEvent>
#include <QVariant>

constexpr QEvent::Type JsConditionEventType = static_cast<QEvent::Type>(QEvent::User + 1);

struct JsConditionEvent : public QEvent {
    JsConditionEvent(const QVariantMap& data, QString& inputKey)
        : QEvent(JsConditionEventType), eventData(data), inputKey(inputKey) {}
    QVariantMap eventData;
    QString inputKey;
};