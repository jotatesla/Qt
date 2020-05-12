#include "QTcpSocketEx.h"

QTcpSocketImpl::QTcpSocketImpl()
    : m_pTcpSocket(nullptr)
{
}

void QTcpSocketImpl::SetSocket(QTcpSocket* pSocket)
{
    m_pTcpSocket = pSocket;

    connect(pSocket, SIGNAL(readyRead()),    this, SLOT(OnReadyRead()));
    connect(pSocket, SIGNAL(disconnected()), this, SLOT(OnDisconnected()));
}

void QTcpSocketImpl::OnReadyRead()
{
    emit ReadyReadEx(m_pTcpSocket);
}

void QTcpSocketImpl::OnDisconnected()
{
    emit DisconnectedEx(m_pTcpSocket);
}
