#include "TcpClientDlg.h"
#include "ui_TcpClientDlg.h"
#include <QAbstractSocket>
#include <QContextMenuEvent>
#include <QDateTime>
#include <QHostAddress>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QTimer>

TcpClientDialog::TcpClientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
    , m_pTcpSocket(nullptr)
    , m_pTimer(nullptr)
{
    ui->setupUi(this);

    // 设置标题
    setWindowTitle("TCP客户端");

    // 去掉问号
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    // 设置端口只允许输入0~65535
    ui->lineEditPort->setValidator(new QIntValidator(0, 65535, this));

    // 设置编辑框和消息记录框的比例
    ui->splitter->setStretchFactor(0, 8);
    ui->splitter->setStretchFactor(2, 2);

    // 初始化TcpSocket（自动释放）
    m_pTcpSocket = new QTcpSocket(this);
    // 初始化定时器（自动释放）
    m_pTimer = new QTimer(this);

    connect(m_pTcpSocket,&QTcpSocket::connected,[=]()
    {
        SetConnectionButtonStatus(true);
        qDebug() << "连接成功";
        m_pTimer->stop();
    });

    connect(m_pTcpSocket,&QTcpSocket::disconnected,[=]()
    {
        SetConnectionButtonStatus(false);
        qDebug() << "连接断开";
    });

    connect(m_pTcpSocket,&QTcpSocket::stateChanged,[=](QAbstractSocket::SocketState state)
    {
        qDebug() << "连接状态发生改变: " << state;
    });

    connect(m_pTcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_error(QAbstractSocket::SocketError)));

    connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(on_Read()));

    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnTimerReconnect()));

    // 设置默认IP
    ui->widgetIPAddress->setIP("192.168.199.70");
    ui->lineEditPort->setText("6800");

    SetConnectionButtonStatus(false);

}

TcpClientDialog::~TcpClientDialog()
{
    delete ui;
}

void TcpClientDialog::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton ret = QMessageBox::question(this, "询问", "是否退出程序？");
    if (ret != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();
        event->accept();
    }
}

void TcpClientDialog::on_pushButtonConnect_clicked()
{
    if (m_pTcpSocket == nullptr)
    {
        return;
    }

    ui->pushButtonConnect->setEnabled(false);

    QAbstractSocket::SocketState state = m_pTcpSocket->state();
    if (state == QAbstractSocket::ConnectedState)
    {
        SetConnectionButtonStatus(true);
        QMessageBox::information(this, "提示", "已连接");
        return;
    }
    else if (state == QAbstractSocket::ConnectingState)
    {
        SetConnectionButtonStatus(true);
        QMessageBox::information(this, "提示", "正在连接");
        return;
    }

    QString strIP = ui->widgetIPAddress->getIP();
    quint16 uiPort = ui->lineEditPort->text().toUInt();

    //主动和服务器建立连接
    m_pTcpSocket->connectToHost(QHostAddress(strIP), uiPort);
    bool bConnect = m_pTcpSocket->waitForConnected(3000);
    if (!bConnect)
    {
        SetConnectionButtonStatus(false);
        QMessageBox::warning(this, "提示", "连接失败");
        return;
    }

    SetConnectionButtonStatus(true);
}

void TcpClientDialog::on_pushButtonDisConnect_clicked()
{
    if (m_pTcpSocket == nullptr)
    {
        return;
    }

    ui->pushButtonDisConnect->setEnabled(false);

    QAbstractSocket::SocketState state = m_pTcpSocket->state();
    if (state == QAbstractSocket::UnconnectedState)
    {
        SetConnectionButtonStatus(false);
        QMessageBox::information(this, "提示", "已断开连接");
        return;
    }

    // 主动和客户端端口断开连接
    m_pTcpSocket->disconnectFromHost();
    m_pTcpSocket->close();

    SetConnectionButtonStatus(false);
}

void TcpClientDialog::on_pushButtonSend_clicked()
{
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
    else if (!m_pTcpSocket->waitForBytesWritten(2000))
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

void TcpClientDialog::on_error(QAbstractSocket::SocketError error)
{
    qDebug() << "连接被断开：" << error;
    switch (error)
    {
        case QAbstractSocket::RemoteHostClosedError :
        {
            if (m_pTimer != nullptr && !m_pTimer->isActive())
            {
                m_pTimer->start(3000);
            }
        }
            break;
        default:
            break;
    }

    SetConnectionButtonStatus(false);
}

void TcpClientDialog::OnTimerReconnect()
{
    QAbstractSocket::SocketState eState = m_pTcpSocket->state();
    if (eState == QAbstractSocket::UnconnectedState)
    {
        qDebug() << "准备重连";
        QString strIP = ui->widgetIPAddress->getIP();
        quint16 uiPort = ui->lineEditPort->text().toUInt();
        m_pTcpSocket->connectToHost(QHostAddress(strIP), uiPort);
    }
}

void TcpClientDialog::on_Read()
{
    // 暂时不考虑黏包的问题
    // 详细请参考 https://blog.csdn.net/Dengdew/article/details/79065608?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-7.nonecase&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-7.nonecase

    QString strText = QString::fromLocal8Bit(m_pTcpSocket->readAll());
    QString strContent = m_pTcpSocket->peerAddress().toString() + ":" + QString::number(m_pTcpSocket->peerPort());
    strContent += "  " + QDateTime::currentDateTime().toString("hh:mm:ss.zzz") + "\r\n";
    strContent += strText + "\r\n";

    ui->textEditRecord->append(strContent);
}

void TcpClientDialog::on_checkBox_stateChanged(int )
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

void TcpClientDialog::SetConnectionButtonStatus(bool bOk)
{
    ui->pushButtonConnect->setEnabled(!bOk);
    ui->pushButtonDisConnect->setEnabled(bOk);
}
