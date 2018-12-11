#include "pch.h"
#include "ClientController.h"
#include "ControllerBase.h"
#include "Client.h"

class CClientController final : public CControllerBase<IClientController, IClient, IClientControllerListener>
{
public:
    virtual ~CClientController() override final = default;
};

IClientControllerUniquePtr IClientController::Create()
{
    return std::make_unique<CClientController>();
}