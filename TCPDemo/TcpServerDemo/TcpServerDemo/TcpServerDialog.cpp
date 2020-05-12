#include "TcpServerDialog.h"
#include "ui_TcpServerDialog.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QNetworkInterface>
#include "QTcpSocketEx.h"
#include "SocketWidget.h"


Q_DECLARE_METATYPE(QTcpSocketImpl*)
Q_DECLARE_METATYPE(SocketWidget*)

// 数据角色
#define DATA_ROLE_SOCKETIMPL                5623
#define DATA_ROLE_SOCKET_WIDGET             5624



TcpServerDialog::TcpServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TcpServerDialog)
    , m_pServer(nullptr)
{
    ui->setupUi(this);

    // 设置标题
    setWindowTitle("TCP Server");

    // 隐藏问号
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    // 设置分割器比例
    ui->splitter->setStretchFactor(0,8);
    ui->splitter->setStretchFactor(1,2);
    ui->widgetList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tabWidget->tabBar()->hide();

    // 加载所有IP
    QList<QHostAddress> lstIP = QNetworkInterface().allAddresses();
    for (const auto& ip : lstIP)
    {
        if(ip.protocol() != QAbstractSocket::IPv4Protocol
           || ip.toString() == "127.0.0.1")
        {
            continue;
        }

        ui->comboBoxIPAddress->addItem(ip.toString());
    }

    // 默认端口
    ui->lineEditPort->setText("6800");

    ui->pushButtonStop->setEnabled(false);

    // 初始化网络
    m_pServer = new QTcpServer(this);


}

TcpServerDialog::~TcpServerDialog()
{
    delete ui;
}

void TcpServerDialog::closeEvent(QCloseEvent* event)
{
    QMessageBox::StandardButton ret = QMessageBox::question(this, "询问", "是否退出程序？");
    if (ret != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        on_pushButtonStop_clicked();
        on_pushButtonAllDisconnect_clicked();

        event->accept();
    }
}


void TcpServerDialog::on_pushButtonListen_clicked()
{
    ui->pushButtonListen->setEnabled(false);

    QString strIP = ui->comboBoxIPAddress->currentText();
    quint16 uiPort = ui->lineEditPort->text().toUInt();
    bool bRet = m_pServer->listen(QHostAddress(strIP), uiPort);
    if (!bRet)
    {
        QMessageBox::warning(this, "提示", "监听失败");
        ui->pushButtonListen->setEnabled(true);
        return;
    }

    connect(m_pServer,SIGNAL(newConnection()),this,SLOT(on_NewClientConnection()));

    ui->pushButtonStop->setEnabled(true);
}

void TcpServerDialog::on_pushButtonStop_clicked()
{
    ui->pushButtonStop->setEnabled(false);

    m_pServer->close();

    ui->pushButtonListen->setEnabled(true);
}

void TcpServerDialog::on_NewClientConnection()
{
    qDebug() << "new client connect";

    QTcpSocket* pSocket = m_pServer->nextPendingConnection(); // 需要释放
    if (pSocket == nullptr)
    {
        qDebug() << "null connection";
        return;
    }
    connect(pSocket,SIGNAL(disconnected()),pSocket,SLOT(deleteLater())); // 设置自动释放

    QTcpSocketImpl* pSocketImpl = new QTcpSocketImpl(); // 需要手动释放
    pSocketImpl->SetSocket(pSocket);
    connect(pSocketImpl, SIGNAL(ReadyReadEx(QTcpSocket*)),    this, SLOT(OnReadyReadEx(QTcpSocket*)));
    connect(pSocketImpl, SIGNAL(DisconnectedEx(QTcpSocket*)), this, SLOT(OnDisconnectedEx(QTcpSocket*)));

    // 创建相应的消息界面分页
    SocketWidget* pSocketWidget = new SocketWidget(); // 需要手动释放

    // 添加到列表
    QString strItem = pSocket->peerAddress().toString() + ":" + QString::number(pSocket->peerPort());
    QListWidgetItem* pItem = new QListWidgetItem(); // 需要手动释放
    pItem->setText(strItem);
    pItem->setData(DATA_ROLE_SOCKETIMPL, QVariant::fromValue(pSocketImpl));
    pItem->setData(DATA_ROLE_SOCKET_WIDGET, QVariant::fromValue(pSocketWidget));
    ui->listWidgetClient->addItem(pItem);

    pSocketWidget->SetSocket(pSocket);

    // 添加到分页
    ui->tabWidget->addTab(pSocketWidget, strItem);
}

void TcpServerDialog::OnReadyReadEx(QTcpSocket* pSocket)
{
    int nCount = ui->listWidgetClient->count();
    for (int i = 0; i < nCount; i++)
    {
        QListWidgetItem* pItem = ui->listWidgetClient->item(i);
        QTcpSocketImpl* pSocketImpl = pItem->data(DATA_ROLE_SOCKETIMPL).value<QTcpSocketImpl*>();
        if (pSocketImpl->m_pTcpSocket == pSocket)
        {
            SocketWidget* pSocketWidget = pItem->data(DATA_ROLE_SOCKET_WIDGET).value<SocketWidget*>();
            if (pSocketWidget == nullptr)
            {
                return;
            }

            pSocketWidget->OnRead();

            break;
        }
    }
}

void TcpServerDialog::OnDisconnectedEx(QTcpSocket* pSocket)
{
    QListWidgetItem* pItem = nullptr;
    int nCount = ui->listWidgetClient->count();
    for (int i = 0; i < nCount; i++)
    {
        pItem = ui->listWidgetClient->item(i);
        QTcpSocketImpl* pSocketImpl = pItem->data(DATA_ROLE_SOCKETIMPL).value<QTcpSocketImpl*>();
        if (pSocketImpl->m_pTcpSocket == pSocket)
        {
            break;
        }
    }

    if (pItem != nullptr)
    {
        DisconnectItem(pItem);
    }
}

void TcpServerDialog::on_listWidgetClient_itemClicked(QListWidgetItem *item)
{
    // 切换到相应的消息界面
    SocketWidget* pSocketWidget = item->data(DATA_ROLE_SOCKET_WIDGET).value<SocketWidget*>();
    if (pSocketWidget == nullptr)
    {
        return;
    }

    ui->tabWidget->setCurrentWidget(pSocketWidget);
}

void TcpServerDialog::on_pushButtonDisconnect_clicked()
{
    QListWidgetItem* pItem = ui->listWidgetClient->currentItem();
    if (pItem == nullptr)
    {
        QMessageBox::warning(this, "提示", "请先选择要操作的连接");
        return;
    }

    SocketWidget* pSocketWidget = pItem->data(DATA_ROLE_SOCKET_WIDGET).value<SocketWidget*>();
    if (pSocketWidget != nullptr)
    {
        // 由回调去释放资源
        pSocketWidget->DoDisconnect();
    }
}

void TcpServerDialog::on_pushButtonAllDisconnect_clicked()
{
    // 断开所有连接
    while(ui->listWidgetClient->count() > 0)
    {
        QListWidgetItem* pItem = ui->listWidgetClient->item(0);
        SocketWidget* pSocketWidget = pItem->data(DATA_ROLE_SOCKET_WIDGET).value<SocketWidget*>();
        if (pSocketWidget != nullptr)
        {
            // 由回调去释放资源
            pSocketWidget->DoDisconnect();
        }
    }
}

void TcpServerDialog::DisconnectItem(QListWidgetItem* pItem)
{
    if (pItem == nullptr)
    {
        return;
    }

    SocketWidget* pSocketWidget = pItem->data(DATA_ROLE_SOCKET_WIDGET).value<SocketWidget*>();
    if (pSocketWidget != nullptr)
    {
        delete pSocketWidget; // 释放消息界面
        pSocketWidget = nullptr;
    }

    QTcpSocketImpl* pSocketImpl = pItem->data(DATA_ROLE_SOCKETIMPL).value<QTcpSocketImpl*>();
    if (pSocketImpl != nullptr)
    {
        delete pSocketImpl; // 释放socket代理
        pSocketImpl = nullptr;
    }

    // 从列表中删除
    int iIndex = ui->listWidgetClient->row(pItem);
    ui->listWidgetClient->takeItem(iIndex);

    // 释放item
    delete pItem;
    pItem = nullptr;
}
