#ifndef TCPSERVERDIALOG_H
#define TCPSERVERDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QTcpServer>



#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif



namespace Ui {
class TcpServerDialog;
}

class TcpServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TcpServerDialog(QWidget *parent = nullptr);
    ~TcpServerDialog();

protected:
    void closeEvent(QCloseEvent* event);


private slots:
    // 监听
    void on_pushButtonListen_clicked();
    // 停止监听
    void on_pushButtonStop_clicked();
    // 新连接
    void on_NewClientConnection();
    // 收到对端消息
    void OnReadyReadEx(QTcpSocket* pSocket);
    // 对端断开连接
    void OnDisconnectedEx(QTcpSocket* pSocket);
    // 点击某项
    void on_listWidgetClient_itemClicked(QListWidgetItem *item);
    // 断开
    void on_pushButtonDisconnect_clicked();
    // 全部断开
    void on_pushButtonAllDisconnect_clicked();

private:
    void DisconnectItem(QListWidgetItem* pItem);


    Ui::TcpServerDialog *ui;

    QTcpServer*         m_pServer;

};

#endif // TCPSERVERDIALOG_H
