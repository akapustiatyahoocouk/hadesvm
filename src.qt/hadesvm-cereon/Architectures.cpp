//
//  hadesvm-cereon/Architectures.cpp
//
//  hadesvm::cereon architectures
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  CereonWorkstationArchitecture
HADESVM_IMPLEMENT_SINGLETON(CereonWorkstationArchitecture)
CereonWorkstationArchitecture::CereonWorkstationArchitecture() {}
CereonWorkstationArchitecture::~CereonWorkstationArchitecture() {}

QString CereonWorkstationArchitecture::mnemonic() const
{
    return "CereonWorkstation";
}

QString CereonWorkstationArchitecture::displayName() const
{
    return "Cereon workstation";
}

//////////
//  hadesvm::core::VirtualArchitecture
void CereonWorkstationArchitecture::validateVirtualAppliance(hadesvm::core::VirtualAppliance * virtualAppliance) throws(hadesvm::core::VirtualApplianceException)
{
    VirtualArchitecture::validateVirtualAppliance(virtualAppliance);    //  may thow

    if (virtualAppliance->componentsImplementing<Processor>().size() < 1 ||
        virtualAppliance->componentsImplementing<Processor>().size() > 256)
    {
        throw hadesvm::core::VirtualApplianceException(displayName() +
                                                       " must contain between 1 and 256 processors");
    }

    if (virtualAppliance->componentsImplementing<MemoryBus>().size() != 1)
    {
        throw hadesvm::core::VirtualApplianceException(displayName() +
                                                       " must contain exactly 1 " +
                                                       MemoryBus::Type::instance()->displayName());
    }

    if (virtualAppliance->componentsImplementing<IoBus>().size() != 1)
    {
        throw hadesvm::core::VirtualApplianceException(displayName() +
                                                       " must contain exactly 1 " +
                                                       IoBus::Type::instance()->displayName());
    }
}

//  End of hadesvm-cereon/Architectures.cpp
