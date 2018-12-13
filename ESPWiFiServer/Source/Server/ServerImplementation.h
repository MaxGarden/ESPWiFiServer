#if !defined(__SERVER_IMPLEMENTATION_H__)
#define __SERVER_IMPLEMENTATION_H__
#pragma once

#include "Server.h"
#include "ListenableBase.h"
#include <QTcpServer>

class CServerImplementation final : public QObject, public CListenableBase<IServer, IServerListener>
{
    Q_OBJECT
public:
    CServerImplementation(QObject* parent = nullptr);
    virtual ~CServerImplementation() override final;

    virtual bool StartListening(unsigned short int port) override final;
    virtual void VisitClients(const ConstVisitorType<IClientSharedPtr>& visitor) override final;

private:
    void Setup();

private slots:
    void OnNewConnection();

private:
    QTcpServer m_Server;
    std::vector<IClientSharedPtr> m_Clients;
};

#endif //__SERVER_IMPLEMENTATION_H__