#include "pch.h"
#include <QApplication>
#include "MainWindow/MainWindow.h"

#include <fstream>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    CMainWindow mainWindow;

    if (!mainWindow.Initialize(1234))
        return -1;

    mainWindow.show();
    return app.exec();
}