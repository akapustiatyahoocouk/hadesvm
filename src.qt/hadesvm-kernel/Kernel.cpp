//
//  hadesvm-kernel/Kernel.cpp
//
//  hadesvm::kernel::Kernel class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
Kernel::Kernel()
    :   _guard(),
        _objects()
{
}

Kernel::~Kernel()
{
}

//////////
//  hadesvm::core::Component
Kernel::Type * Kernel::type() const
{
    return Type::instance();
}

QString Kernel::displayName() const
{
    return Type::instance()->displayName();
}

void Kernel::serialiseConfiguration(QDomElement componentElement)
{
}

void Kernel::deserialiseConfiguration(QDomElement componentElement)
{
}

//  End of hadesvm-kernel/Kernel.cpp
