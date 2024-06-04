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
        _live(true),
        _referenceCount(0)
{
    Q_ASSERT(_kernel != nullptr);
    Q_ASSERT(_kernel->isLockedByCurrentThread());
    Q_ASSERT(!_kernel->_liveObjects.contains(_oid));

    kernel->_liveObjects[_oid] = this;
    _referenceCount++;  //  we've just created a reference to this Object!
}

Object::~Object()
{
    Q_ASSERT(_referenceCount == 0);
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

//////////
//  Operations
Kernel * Object::kernel() const
{
    Q_ASSERT(_kernel->isLockedByCurrentThread());

    return _kernel;
}

Oid Object::oid() const
{
    Q_ASSERT(_kernel->isLockedByCurrentThread());

    return _oid;
}

bool Object::live() const
{
    Q_ASSERT(_kernel->isLockedByCurrentThread());

    return _live;
}

int Object::referenceCount() const
{
    Q_ASSERT(_kernel->isLockedByCurrentThread());

    return _referenceCount;
}

void Object::incrementReferenceCount()
{
    Q_ASSERT(_kernel->isLockedByCurrentThread());

    _referenceCount++;
    Q_ASSERT(_referenceCount != 0);
}

void Object::decrementReferenceCount()
{
    Q_ASSERT(_kernel->isLockedByCurrentThread());

    Q_ASSERT(_referenceCount != 0);
    _referenceCount--;
}

//  End of hadesvm-kernel/Object.cpp
