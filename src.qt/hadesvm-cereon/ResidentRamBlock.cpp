//
//  hadesvm-cereon/ResidentRamBlock.cpp
//
//  hadesvm::cereon::ResidentRamBlock class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Constants
const uint64_t   ResidentRamBlock::DefaultStartAddress = 0;
const MemorySize ResidentRamBlock::DefaultSize = MemorySize::megabytes(64);

//////////
//  Construction/destruction
ResidentRamBlock::ResidentRamBlock()
    :   ResidentMemoryBlock(DefaultStartAddress, DefaultSize)
{
}

ResidentRamBlock::~ResidentRamBlock() noexcept
{
}

//////////
//  hadesvm::core::Component
QString ResidentRamBlock::displayName() const
{
    return hadesvm::util::toString(size()) +
           " RAM @ " +
           hadesvm::util::toString(startAddress(), "%016X");
}

ComponentEditor * ResidentRamBlock::createEditor(QWidget * parent)
{
    return new ResidentRamBlockEditor(parent, this);
}

ResidentRamBlock::Ui * ResidentRamBlock::createUi()
{
    return nullptr;
}

//////////
//  ResidentRamBlock::Type
HADESVM_IMPLEMENT_SINGLETON(ResidentRamBlock::Type)
ResidentRamBlock::Type::Type() {}
ResidentRamBlock::Type::~Type() {}

QString ResidentRamBlock::Type::mnemonic() const
{
    return "CereonResidentRamBlock";
}

QString ResidentRamBlock::Type::displayName() const
{
    return "Cereon Resident RAM Block";
}

hadesvm::core::ComponentCategory * ResidentRamBlock::Type::category() const
{
    return StandardComponentCategories::Memory;
}

bool ResidentRamBlock::Type::suspendable() const
{
    return true;
}

bool ResidentRamBlock::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool ResidentRamBlock::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

ResidentRamBlock * ResidentRamBlock::Type::createComponent()
{
    return new ResidentRamBlock();
}

//  End of hadesvm-cereon/ResidentMemoryBlock.cpp
