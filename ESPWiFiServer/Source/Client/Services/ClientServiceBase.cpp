#include "pch.h"
#include "ClientServiceBase.h"

void CClientServiceBase::OnBind(const IClientServiceConnectionSharedPtr& connection)
{
    DEBUG_ASSERT(!m_Connection);
    if (m_Connection)
        OnUnbind(m_Connection);

    m_Connection = connection;
}

void CClientServiceBase::OnUnbind(const IClientServiceConnectionSharedPtr& connection)
{
    DEBUG_ASSERT(m_Connection == connection);
    if (m_Connection == connection)
        m_Connection.reset();
}

void CClientServiceBase::OnReceived(const std::vector<byte>&)
{
    //to override
}

bool CClientServiceBase::Send(std::vector<byte>&& payload)
{
    const auto& connection = GetConnection();
    DEBUG_ASSERT(connection);
    if (!connection)
        return false;

    const auto result = connection->Send(std::move(payload));
    DEBUG_ASSERT(result);
    return result;
}

const IClientServiceConnectionSharedPtr& CClientServiceBase::GetConnection() const noexcept
{
    return m_Connection;
}