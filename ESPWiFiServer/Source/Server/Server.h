#if !defined(__SERVER_H__)
#define __SERVER_H__
#pragma once

#include <QTcpServer>

class CServer final : public QObject
{
    Q_OBJECT
public:
    CServer(QObject* parent = nullptr);
    virtual ~CServer() override final;

    bool StartListening(unsigned short int port);

private:
    void Setup();

private slots:
    void OnNewConnection();

private:
    QTcpServer m_server;
};

#endif //__SERVER_H__