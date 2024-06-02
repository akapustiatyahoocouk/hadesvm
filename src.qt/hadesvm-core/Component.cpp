//
//  hadesvm-core/Component.cpp
//
//  hadesvm::core::Component class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Construction/destruction
Component::Component()
{
}

Component::~Component()
{
    Q_ASSERT(_virtualAppliance == nullptr); //  must be a free component
}

//  End of hadesvm-core/Component.cpp
