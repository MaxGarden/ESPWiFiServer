#include "pch.h"
#include <QApplication>
#include "MainWindow/MainWindow.h"

#include <fstream>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    CMainWindow mainWindow;

    mainWindow.show();
    return app.exec();
}