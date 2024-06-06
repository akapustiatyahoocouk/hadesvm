//
//  hadesvm-kernel/Atom.cpp
//
//  hadesvm::kernel::Atom class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
Atom::Atom(Kernel * kernel, const QString & name)
    :   Object(kernel),
        _name(name),
        _atomInterests()
{
    Q_ASSERT(kernel->isLockedByCurrentThread());
    Q_ASSERT(!kernel->_atomsByName.contains(name));

    kernel->_atomsByName.insert(name, this);
    this->incrementReferenceCount();    //  we've jyst created a reference to this Atom
}

Atom::~Atom()
{
    Kernel * kernel = this->kernel();
    Q_ASSERT(kernel->isLockedByCurrentThread());

    Q_ASSERT(kernel->isLockedByCurrentThread());
    Q_ASSERT(!kernel->_atomsByName.contains(_name) && kernel->_atomsByName[_name] == this);

    kernel->_atomsByName.remove(_name);
    this->decrementReferenceCount();    //  we've just dropped a reference to this Atom
}

//  End of hadesvm-kernel/Atom.cpp
