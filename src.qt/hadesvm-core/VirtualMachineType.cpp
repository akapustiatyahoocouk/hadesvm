//
//  hadesvm-core/VirtualMachineType.cpp
//
//  hadesvm::core::VirtualMachineType class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Singleton
HADESVM_IMPLEMENT_SINGLETON(VirtualMachineType)

VirtualMachineType::VirtualMachineType() {}
VirtualMachineType::~VirtualMachineType() {}

//////////
//  hadesvm::util::StockObject
QString VirtualMachineType::mnemonic() const
{
    return "VirtualMachine";
}

QString VirtualMachineType::displayName() const
{
    return "Virtual machine";
}

//////////
//  VirtualApplianceType
VirtualAppliance * VirtualMachineType::createVirtualAppliance(const QString & name, const QString & location,
                                                              VirtualArchitecture * architecture)
{
    return createVirtualMachine(name, location, architecture);
}

//////////
//  Operations
VirtualMachine * VirtualMachineType::createVirtualMachine(const QString & name, const QString & location,
                                                          VirtualArchitecture * architecture)
{
    return new VirtualMachine(name, location, architecture);
}

//////////
//  Registry
bool VirtualMachineType::register(VirtualMachineType * virtualMachineType)
{
    return VirtualApplianceType::register(virtualMachineType);
}

VirtualMachineType * VirtualMachineType::findByMnemonic(const QString & mnemonic)
{
    return dynamic_cast<VirtualMachineType*>(VirtualApplianceType::findByMnemonic(mnemonic));
}

VirtualMachineTypeList VirtualMachineType::all()
{
    VirtualMachineTypeList result;
    for (auto rvat : VirtualApplianceType::all())
    {
        if (auto rvmt = dynamic_cast<VirtualMachineType*>(rvat))
        {
            result.append(rvmt);
        }
    }
    return result;
}

//  End of hadesvm-core/VirtualMachineType.cpp
