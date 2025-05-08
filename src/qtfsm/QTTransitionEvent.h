#pragma once
#include <QEvent>
#include <QVariant>

class JsConditionEvent : public QEvent {
public:
    static constexpr QEvent::Type EventType = static_cast<QEvent::Type>(QEvent::User + 1);

    JsConditionEvent(const QVariantMap& data = {})
        : QEvent(EventType), eventData(data) {}

    QVariantMap eventData;
};