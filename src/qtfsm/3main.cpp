#include "QTfsmBuilder.h"
#include "../fsm/FSM.h"
#include "../io/JsonMaker.h"
#include <qfile.h>

int main() {

    QTfsmBuilder* builder = new QTfsmBuilder();

    QString fileName = "generated_fsm.json";
    QFile file(fileName);
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    const QJsonDocument& jsonRef = jsonDoc;
    builder->buildQTfsm(jsonRef);

    QTfsm fsm = builder->getBuiltFsm();

    fsm.start();

}