//
//  hadesvm-cereon/ResidentRamUnit.cpp
//
//  hadesvm::cereon::ResidentRamUnit class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Constants
const uint64_t   ResidentRamUnit::DefaultStartAddress = 0;
const hadesvm::core::MemorySize ResidentRamUnit::DefaultSize = hadesvm::core::MemorySize::megabytes(64);

//////////
//  Construction/destruction
ResidentRamUnit::ResidentRamUnit()
    :   ResidentMemoryUnit(DefaultStartAddress, DefaultSize)
{
}

ResidentRamUnit::~ResidentRamUnit() noexcept
{
}

//////////
//  hadesvm::core::Component
QString ResidentRamUnit::displayName() const
{
    return hadesvm::util::toString(size()) +
           " RAM @ " +
           hadesvm::util::toString(startAddress(), "%016X");
}

hadesvm::core::ComponentEditor * ResidentRamUnit::createEditor(QWidget * parent)
{
    return new ResidentRamUnitEditor(parent, this);
}

ResidentRamUnit::Ui * ResidentRamUnit::createUi()
{
    return nullptr;
}

//////////
//  ResidentRamUnit::Type
HADESVM_IMPLEMENT_SINGLETON(ResidentRamUnit::Type)
ResidentRamUnit::Type::Type() {}
ResidentRamUnit::Type::~Type() {}

QString ResidentRamUnit::Type::mnemonic() const
{
    return "CereonResidentRamUnit";
}

QString ResidentRamUnit::Type::displayName() const
{
    return "Cereon Resident RAM Unit";
}

hadesvm::core::ComponentCategory * ResidentRamUnit::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::Memory;
}

bool ResidentRamUnit::Type::suspendable() const
{
    return true;
}

bool ResidentRamUnit::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool ResidentRamUnit::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

ResidentRamUnit * ResidentRamUnit::Type::createComponent()
{
    return new ResidentRamUnit();
}

//  End of hadesvm-cereon/ResidentRamUnit.cpp
