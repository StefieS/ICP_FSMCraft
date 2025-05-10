#include "QTBuiltinHandler.h"
#include <QDebug>
#include "QTfsm.h"

QTBuiltinHandler::QTBuiltinHandler(QObject* parent, QTfsm* fsm)
    : QObject(parent), fsm(fsm) {
}


void QTBuiltinHandler::output(const QString& name, const QString& value) {
    qDebug() << "JS output:" << name << "=" << value;
    
   fsm->setOutput(name, value);
}

int QTBuiltinHandler::elapsed() {
    return stateTimer.isValid() ? stateTimer.elapsed() : 0;
}

void QTBuiltinHandler::stateEntered(QState* newState) {
    if (newState != lastActiveState) {
        stateTimer.restart();
        lastActiveState = newState;
    }
}