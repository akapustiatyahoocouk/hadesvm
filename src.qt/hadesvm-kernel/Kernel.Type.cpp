//
//  hadesvm-kernel/Kernel.Type.cpp
//
//  hadesvm::kernel::Kernel::Type class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Singleton
HADESVM_IMPLEMENT_SINGLETON(Kernel::Type)
Kernel::Type::Type() {}
Kernel::Type::~Type() {}

//////////
//  hadesvm::util::StockObject
QString Kernel::Type::mnemonic() const
{
    return "HadesVM.Kernel";
}

QString Kernel::Type::displayName() const
{
    return "HADES VM kernel";
}

//////////
//  hadesvn::core::ComponentType
hadesvm::core::ComponentCategory * Kernel::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::Software;
}

bool Kernel::Type::suspendable() const
{
    return false;
}

bool Kernel::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == Architecture::instance();
}

bool Kernel::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

hadesvm::core::Component * Kernel::Type::createComponent()
{
    return new Kernel("LOCAL");
}

//  End of hadesvm-kernel/Kernel.Type.cpp
