#include "TcpServerDialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpServerDialog w;
    w.show();

    return a.exec();
}
