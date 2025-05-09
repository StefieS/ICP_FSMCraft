#include "FsmController.h"
#include "../../qtfsm/QTConditionEvent.h"

FsmController::FsmController() {}

const Message& FsmController::performAction(Message &msg) {

    EMessageType type = msg.getType();
    Message response = Message();
    switch (type)
    {
    case (EMessageType::JSON) : {
        // creating running QTfsm
        std::string name = msg.getJsonName();
        QTfsmBuilder* builder = new QTfsmBuilder();

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
        const QJsonDocument& jsonRef = jsonDoc;
        builder->buildQTfsm(jsonRef);
    
        this->qtfsm = builder->getBuiltFsm();
        this->qtfsm->start();
        response.buildAcceptMessage();
        return response;
    }

    case (EMessageType::INPUT) : {

        // posting input event
        std::string name = msg.getInputName();
        std::string value = msg.getInputValue();
        QVariantMap data;
        QString qName = QString::fromStdString(name);
        QString qValue = QString::fromStdString(value);
        data[qName] = qValue;
        this->qtfsm->postEvent(new JsConditionEvent(data, qName));
        
        return response;
    }

    case (EMessageType:: STOP) : {
        
        break;
    }
    
    default:
        return response;
    }
}