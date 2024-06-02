//
//  hadesvm-core/Exceptions.cpp
//
//  hadesvm-core exceptions implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  VirtualApplianceException
VirtualApplianceException::VirtualApplianceException(const QString & message)
    :   hadesvm::util::Exception(message) {}

VirtualApplianceException::VirtualApplianceException(const std::string & message)
    :   hadesvm::util::Exception(message) {}

VirtualApplianceException::VirtualApplianceException(const char * message)
    :   hadesvm::util::Exception(message) {}

//  End of hadesvm-core/Exceptions.cpp
