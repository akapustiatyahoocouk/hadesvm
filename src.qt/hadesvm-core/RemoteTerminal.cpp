//
//  hadesvm-core/RemoteTerminal.cpp
//
//  hadesvm::core::RemoteTerminal class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Construction/destruction
RemoteTerminal::RemoteTerminal(const QString & name, const QString & location,
                               VirtualArchitecture * architecture)
    :   VirtualAppliance(name, location, architecture)
{
}

RemoteTerminal::~RemoteTerminal()
{
}

//////////
//  VirtualAppliance
VirtualApplianceType * RemoteTerminal::type() const
{
    return RemoteTerminalType::instance();
}

//  End of hadesvm-core/RemoteTerminal.cpp
