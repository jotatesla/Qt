#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile qssFile(":/style.qss");
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen())
    {
        QString strQss = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(strQss);
        qssFile.close();
    }

    MainWindow w;
    w.show();

    return a.exec();
}
