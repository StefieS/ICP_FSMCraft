#pragma once

#include <QAbstractTransition>
#include <QString>
#include <QEvent>
#include "../messages/Message.h"
#include "QTfsm.h"
class QTTransitionEnd : public QAbstractTransition {
private:
    QTfsm* automaton;
public:
    QTTransitionEnd(QState* parentState, QTfsm* fsm): 
        QAbstractTransition(parentState),
        automaton(fsm){}

protected:
    void onTransition(QEvent*) override {
        Message msg = Message();
        msg.buildStopMessage();
        automaton->getNetworkHandler().sendToHost(msg.toMessageString());
    }
};
