//
//  hadesvm-cereon/Cereon1P1B.cpp
//
//  hadesvm::cereon::Cereon1P1B class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Constants
const ByteOrder      Cereon1P1B::DefaultByteOrder = ByteOrder::BigEndian;
const uint64_t       Cereon1P1B::DefaultRestartAddress = UINT64_C(0xFFFFFFFFFFF00000);
const hadesvm::core::ClockFrequency Cereon1P1B::DefaultClockFrequency = hadesvm::core::ClockFrequency::megahertz(20);

//////////
//  Construction/destruction
Cereon1P1B::Cereon1P1B(uint8_t id)
    :   Processor(DefaultClockFrequency, id, DefaultByteOrder, false, DefaultRestartAddress, true),
        //  Processor blocks
        _mmu(),
        _core(this, 0, Feature::Base, DefaultByteOrder, false, &_mmu)
{
}

Cereon1P1B::~Cereon1P1B() noexcept
{
}

//////////
//  Cereon1P1B::Type
HADESVM_IMPLEMENT_SINGLETON(Cereon1P1B::Type)
Cereon1P1B::Type::Type() {}
Cereon1P1B::Type::~Type() {}

QString Cereon1P1B::Type::mnemonic() const
{
    return "Cereon1P1B";
}

QString Cereon1P1B::Type::displayName() const
{
    return "Cereon-1P1B";
}

hadesvm::core::ComponentCategory * Cereon1P1B::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::Processors;
}

bool Cereon1P1B::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool Cereon1P1B::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

Cereon1P1B * Cereon1P1B::Type::createComponent()
{
    return new Cereon1P1B(0);
}

//  End of hadesvm-cereon/Cereon1P1B.cpp
