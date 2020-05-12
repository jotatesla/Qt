#include "SocketWidget.h"
#include "ui_SocketDialog.h"
#include <QDateTime>
#include <QHostAddress>
#include <QMessageBox>

SocketWidget::SocketWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SocketDialog)
    , m_pTcpSocket(nullptr)
{
    ui->setupUi(this);
}

SocketWidget::~SocketWidget()
{
    delete ui;
}

void SocketWidget::SetSocket(QTcpSocket* pSocket)
{
    m_pTcpSocket = pSocket;
}

void SocketWidget::OnRead()
{
    if (m_pTcpSocket == nullptr)
    {
        return;
    }

    QString strText = QString::fromLocal8Bit(m_pTcpSocket->readAll());
    QString strContent = m_pTcpSocket->peerAddress().toString() + ":" + QString::number(m_pTcpSocket->peerPort());
    strContent += "  " + QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "\r\n";
    strContent += strText + "\r\n";

    ui->textEditRecord->append(strContent);
}

void SocketWidget::DoDisconnect()
{
    if (m_pTcpSocket == nullptr)
    {
        return;
    }

    QAbstractSocket::SocketState state = m_pTcpSocket->state();
    if (state == QAbstractSocket::UnconnectedState)
    {
        return;
    }

    // 主动和客户端端口断开连接
    m_pTcpSocket->disconnectFromHost();
//    m_pTcpSocket->close();
}

void SocketWidget::on_pushButtonSend_clicked()
{
    if (m_pTcpSocket == nullptr)
    {
        return;
    }

    QString strText = ui->textEditSend->toPlainText();
    if (strText.isEmpty())
    {
        return;
    }

    QByteArray bySend;
    if (ui->checkBox->checkState() == Qt::CheckState::Checked)
    {
        bySend = QByteArray::fromHex(strText.toLatin1());
    }
    else
    {
        bySend = strText.toLocal8Bit();
    }

    qint64 nLen = m_pTcpSocket->write(bySend);
    if (nLen < 0)
    {
        QMessageBox::warning(this, "提示", "发送失败");
        return;
    }

    // 更新消息记录
    QString strContent = m_pTcpSocket->localAddress().toString() + ":" + QString::number(m_pTcpSocket->localPort());
    strContent += "  " + QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "\r\n";
    QString strUtf8 = QString::fromLocal8Bit(bySend);
    for (const auto& item : strUtf8)
    {
        QChar ch(item);
        if(ch.isPrint())
        {
            strContent += item;
        }
        else
        {
            strContent += '?';
        }
    }
    strContent += "\r\n";

    ui->textEditRecord->append(strContent);
}

void SocketWidget::on_checkBox_stateChanged(int arg1)
{
    QString strText = ui->textEditSend->toPlainText();

    if (ui->checkBox->checkState() == Qt::CheckState::Checked)
    {
        // 转为16进制
        QString strHex = strText.toLatin1().toHex(' ');
        ui->textEditSend->setText(strHex);
    }
    else
    {
        // 转为普通字符串
        QByteArray byteNormal = QByteArray::fromHex(strText.toLatin1());
        ui->textEditSend->setPlainText(byteNormal);
    }
}
