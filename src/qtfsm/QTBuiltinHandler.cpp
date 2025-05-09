#include "QTBuiltinHandler.h"
#include <QDebug>

QTBuiltinHandler::QTBuiltinHandler(QObject* parent)
    : QObject(parent) {
    // Any initialization code can go here
}

// Slot implementation
void QTBuiltinHandler::output(const QString& name, const QString& value) {
    qDebug() << "JS output:" << name << "=" << value;
    
    // TODO: Handle the output (e.g., storing or processing it further)
}
