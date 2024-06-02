//
//  hadesvm-core/RemoteTerminalType.cpp
//
//  hadesvm::core::RemoteTerminalType class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Singleton
HADESVM_IMPLEMENT_SINGLETON(RemoteTerminalType)

RemoteTerminalType::RemoteTerminalType() {}
RemoteTerminalType::~RemoteTerminalType() {}

//////////
//  hadesvm::util::StockObject
QString RemoteTerminalType::mnemonic() const
{
    return "RemoteTerminal";
}

QString RemoteTerminalType::displayName() const
{
    return "Remote terminal";
}

//////////
//  VirtualApplianceType
VirtualAppliance * RemoteTerminalType::createVirtualAppliance(const QString & name, const QString & location,
                                                              VirtualArchitecture * architecture)
{
    return createRemoteTerminal(name, location, architecture);
}

//////////
//  Operations
RemoteTerminal * RemoteTerminalType::createRemoteTerminal(const QString & name, const QString & location,
                                                          VirtualArchitecture * architecture)
{
    return new RemoteTerminal(name, location, architecture);
}

//////////
//  Registry
bool RemoteTerminalType::register(RemoteTerminalType * remoteTerminalType)
{
    return VirtualApplianceType::register(remoteTerminalType);
}

RemoteTerminalType * RemoteTerminalType::findByMnemonic(const QString & mnemonic)
{
    return dynamic_cast<RemoteTerminalType*>(VirtualApplianceType::findByMnemonic(mnemonic));
}

RemoteTerminalTypeList RemoteTerminalType::all()
{
    RemoteTerminalTypeList result;
    for (auto rvat : VirtualApplianceType::all())
    {
        if (auto rvmt = dynamic_cast<RemoteTerminalType*>(rvat))
        {
            result.append(rvmt);
        }
    }
    return result;
}

//  End of hadesvm-core/RemoteTerminalType.cpp
