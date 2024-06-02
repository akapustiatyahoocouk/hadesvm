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
RemoteTerminal::RemoteTerminal(const QString & name, const QString & location)
    :   VirtualAppliance(name, location)
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
