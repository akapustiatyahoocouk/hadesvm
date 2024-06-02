//
//  hadesvm-core/ComponentAdaptor.cpp
//
//  hadesvm::core::ComponentAdaptor class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Construction/destruction
ComponentAdaptor::ComponentAdaptor(Component * adaptedComponent)
    :   _adaptedComponent(adaptedComponent)
{
    Q_ASSERT(_adaptedComponent != nullptr);
}

ComponentAdaptor::~ComponentAdaptor()
{
    Q_ASSERT(_virtualAppliance == nullptr); //  must be a free component
}

//  End of hadesvm-core/ComponentAdaptor.cpp
