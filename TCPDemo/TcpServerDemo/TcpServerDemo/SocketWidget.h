#ifndef SOCKETDIALOG_H
#define SOCKETDIALOG_H

#include <QTcpSocket>
#include <QWidget>

namespace Ui {
class SocketDialog;
}

class SocketWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SocketWidget(QWidget *parent = nullptr);
    ~SocketWidget();


    void SetSocket(QTcpSocket* pSocket);

    // 读数据
    void OnRead();

    // 本地断开连接
    void DoDisconnect();

private slots:
    void on_pushButtonSend_clicked();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::SocketDialog *ui;

    QTcpSocket*         m_pTcpSocket;
};

#endif // SOCKETDIALOG_H
