#include "TcpClientDlg.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClientDialog w;
    w.show();

    return a.exec();
}
