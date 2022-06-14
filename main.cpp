#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication connect4(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return connect4.exec();
}
