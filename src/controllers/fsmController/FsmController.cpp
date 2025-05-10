#include "FsmController.h"
#include "../../qtfsm/QTConditionEvent.h"

FsmController::FsmController() : qtfsm(nullptr) {}

const Message FsmController::performAction(Message &msg) {
    EMessageType type = msg.getType();
    Message response;

    switch (type) {
    case EMessageType::JSON: {
        std::string name = msg.getJsonName();
        QString fileName = QString::fromStdString(name);
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Couldn't open the file for reading:" << file.errorString();
            response.buildRejectMessage("Couldn't open the file for reading");
            return response;
        }

        QByteArray jsonData = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error != QJsonParseError::NoError || jsonDoc.isNull()) {
            std::string errorMsg = "JSON parse error: " + parseError.errorString().toStdString();
            response.buildRejectMessage(errorMsg);
            return response;
        }

        std::unique_ptr<QTfsmBuilder> builder = std::make_unique<QTfsmBuilder>();
        builder->buildQTfsm(jsonDoc);
        this->qtfsm = builder->getBuiltFsm();

        if (!this->qtfsm) {
            response.buildRejectMessage("Failed to build FSM.");
            return response;
        }
        response.buildAcceptMessage();
        this->qtfsm->start();
        return response;
    }

    case EMessageType::INPUT: {
        if (!this->qtfsm) {
            response.buildRejectMessage("FSM not initialized.");
            return response;
        }

        std::string name = msg.getInputName();
        std::string value = msg.getInputValue();
        QVariantMap data;
        QString qName = QString::fromStdString(name);
        QString qValue = QString::fromStdString(value);
        data[qName] = qValue;

        this->qtfsm->postEvent(new JsConditionEvent(data, qName));
        return response;
    }

    case EMessageType::STOP: {
        if (!this->qtfsm) {
            response.buildRejectMessage("FSM not initialized.");
            return response;
        }

        this->qtfsm->stop();
        return response;
    }

    case EMessageType::REQUEST: {
        if (!this->qtfsm) {
            response.buildRejectMessage("FSM not initialized.");
            return response;
        }

        response.buildJsonMessage(this->qtfsm->getName());
        return response;

    }

    case EMessageType::REJECT: {
        if (!this->qtfsm) {
            return response;
        }
        this->qtfsm->stop();
        return response;
    }

    default:
        return response;
    }
}
