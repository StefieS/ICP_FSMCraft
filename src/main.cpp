#include <QApplication>
#include "./gui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    
    window.show();

    app.exec();
    window.listenerThread.join();
    return 0;
}

