#ifndef TCPCLIENTDLG_H
#define TCPCLIENTDLG_H

#include <QDialog>
#include <QTcpSocket>


// 设置UTF编码
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif


namespace Ui {
class Dialog;
}

class TcpClientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TcpClientDialog(QWidget *parent = nullptr);
    ~TcpClientDialog();

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void on_pushButtonConnect_clicked();

    void on_pushButtonDisConnect_clicked();

    void on_pushButtonSend_clicked();

    void on_error(QAbstractSocket::SocketError err);

    void OnTimerReconnect();

    void on_Read();

    void on_checkBox_stateChanged(int arg1);

private:
    void SetConnectionButtonStatus(bool bOk);

    Ui::Dialog *ui;

    QTcpSocket*         m_pTcpSocket;
    QTimer*             m_pTimer;
};

#endif // TCPCLIENTDLG_H
