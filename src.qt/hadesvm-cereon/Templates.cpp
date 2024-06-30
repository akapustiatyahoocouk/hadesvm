//
//  hadesvm-cereon/Templates.cpp
//
//  hadesvm::cereon VA templates
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Singleton
HADESVM_IMPLEMENT_SINGLETON(CereonWorkstationMicroTemplate)
CereonWorkstationMicroTemplate::CereonWorkstationMicroTemplate() {}
CereonWorkstationMicroTemplate::~CereonWorkstationMicroTemplate() {}

//////////
//  hadesvm::util::StockObject
QString CereonWorkstationMicroTemplate::mnemonic() const
{
    return "CereonWorkstationMicro";
}

QString CereonWorkstationMicroTemplate::displayName() const
{
    return "Cereon workstation Micro";
}

//////////
//  hadesvm::core::VirtualApplianceTemplate
hadesvm::core::VirtualApplianceType * CereonWorkstationMicroTemplate::virtualApplianceType() const
{
    return hadesvm::core::VirtualMachineType::instance();
}

CereonWorkstationArchitecture * CereonWorkstationMicroTemplate::virtualArchitecture() const
{
    return CereonWorkstationArchitecture::instance();
}

//  End of hadesvm-cereon/Templates.cpp
