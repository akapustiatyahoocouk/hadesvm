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

//  End of hadesvm-kernel/Kernel.cpp
