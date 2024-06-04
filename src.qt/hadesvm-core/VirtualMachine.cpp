//
//  hadesvm-core/VirtualMachine.cpp
//
//  hadesvm::core::VirtualMachine class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Construction/destruction
VirtualMachine::VirtualMachine(const QString & name, const QString & location,
                               VirtualArchitecture * architecture)
        :   VirtualAppliance(name, location, architecture)
{
}

VirtualMachine::~VirtualMachine()
{
}

//////////
//  VirtualAppliance
VirtualApplianceType * VirtualMachine::type() const
{
    return VirtualMachineType::instance();
}

//  End of hadesvm-core/VirtualMachine.cpp
