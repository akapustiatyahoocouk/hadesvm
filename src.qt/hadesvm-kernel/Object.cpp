//
//  hadesvm-kernel/Object.cpp
//
//  hadesvm::kernel::Object class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
Object::Object(Kernel * kernel)
    :   _kernel(kernel),
        _oid(kernel->_generateUniqueOid()), //  checks that current thread has locked the kernel
        _live(true)
{
    Q_ASSERT(!kernel->_liveObjects.contains(_oid));
    kernel->_liveObjects[_oid] = this;
}

Object::~Object()
{
    Q_ASSERT(_kernel->isLockedByCurrentThread());
    Q_ASSERT(_kernel->_liveObjects.contains(_oid) || _kernel->_deadObjects.contains(_oid));

    if (_kernel->_liveObjects.contains(_oid))
    {
        _kernel->_liveObjects.remove(_oid);
    }
    else
    {
        _kernel->_deadObjects.remove(_oid);
    }
}

//  End of hadesvm-kernel/Object.cpp
