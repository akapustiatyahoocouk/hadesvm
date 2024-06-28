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
}

//  End of hadesvm-core/ComponentAdaptor.cpp
