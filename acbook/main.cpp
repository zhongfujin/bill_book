#include "mainwindow.h"
#include <QApplication>
#include "login.h"
#include <synchapi.h>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow();

    return a.exec();
}
