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

//  End of hadesvm-cereon/Architectures.cpp
