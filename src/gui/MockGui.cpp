#include "MockGui.h"
#include <QDebug>
#include <iostream>
void MockGui::printLog(std::string logMessage) {
    qDebug() << QString::fromStdString(logMessage);
}

void MockGui::showOutput(std::string outputID, std::string outputValue) {
    qDebug() << "OUTPUT" << QString::fromStdString(outputID) << ":" << QString::fromStdString(outputValue);
}

void MockGui::showError(std::string errorMessage) {
    qDebug() << "ERROR" << QString::fromStdString(errorMessage);
} 

void MockGui::setRunning() {
    qDebug() << "SET RUNNING";
    this->running = true;
}

void MockGui::loadFSMFromJson(std::string pathToJson) {
    return;
}

void MockGui::highlightItem(bool on, IActivable& item) {
    qDebug() << "SET ACTIVABLE" << on;
}

void MockGui::startInputThread(InputQueue& inputQueue) {
    std::thread([&inputQueue]() {
        while (true) {
            std::string name, value;
            std::cout << "Enter input (name value): ";
            std::cin >> name >> value;

            inputQueue.push({name, value});
        }
    }).detach(); // Detach to keep running in the background
}

