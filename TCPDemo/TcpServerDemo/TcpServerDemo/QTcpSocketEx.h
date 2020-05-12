#ifndef QTCPSOCKETEX_H
#define QTCPSOCKETEX_H

#include <QTcpSocket>

class QTcpSocketImpl : public QObject
{
    Q_OBJECT

public:
    QTcpSocketImpl();


    void SetSocket(QTcpSocket* pSocket);

private slots:
    void OnReadyRead();
    void OnDisconnected();

signals:
    void ReadyReadEx(QTcpSocket* pSocket);
    void DisconnectedEx(QTcpSocket* pSocket);

public:
    QTcpSocket*         m_pTcpSocket;
};

#endif // QTCPSOCKETEX_H
