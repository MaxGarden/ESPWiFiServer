#if !defined(__SERVER_H__)
#define __SERVER_H__
#pragma once

#include <QTcpServer>
#include "Client/Client.h"

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
    QTcpServer m_Server;
    std::vector<IClientSharedPtr> m_Clients;
};

#endif //__SERVER_H__