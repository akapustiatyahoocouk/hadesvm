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
    :   _stateGuard()
{
}

Component::~Component()
{
    Q_ASSERT(state() != State::Running);
    Q_ASSERT(_virtualAppliance == nullptr); //  must be a free component
}

//////////
//  Operations (state management)
Component::State Component::state() const
{
    QMutexLocker lock(&_stateGuard);
    return _state;
}

//  End of hadesvm-core/Component.cpp
