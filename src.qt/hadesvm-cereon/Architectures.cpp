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

    QList<Processor*> processors = virtualAppliance->componentsImplementing<Processor>();
    if (processors.size() < 1 || processors.size() > 256)
    {
        throw hadesvm::core::VirtualApplianceException(displayName() +
                                                       " must contain between 1 and 256 processors");
    }
    //  TODO no processor ID conflicts!!!
    int numPrimaryProcessors = 0;
    for (Processor * processor : processors)
    {
        if (processor->isPrimaryProcessor())
        {
            numPrimaryProcessors++;
        }
    }
    if (numPrimaryProcessors != 1)
    {
        throw hadesvm::core::VirtualApplianceException(displayName() +
                                                       " must contain exactly 1 primary processor");
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
