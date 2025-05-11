/**
 * @file main.cpp
 * @brief Main file of the program starting the execution.
 * @author xlesigm00
 * @author xnovakf00
 * @author
 */

#include <QApplication>
#include "./gui/MainWindow.h"

/**
 * @brief Starts the execution of QApplication
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    
    window.show();

    app.exec();

    return 0;
}

