//
//  hadesvm-kernel/Kernel.SystemCalls.cpp
//
//  hadesvm::kernel::Kernel::SystemCalls class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction - from friends only
Kernel::SystemCalls::SystemCalls(Kernel * kernel)
    :   _kernel(kernel)
{
    Q_ASSERT(kernel != nullptr);
}

Kernel::SystemCalls::~SystemCalls()
{
}

//////////
//  Operations (atoms)
KErrno Kernel::SystemCalls::getAtom(Thread * thread, const QString & name, Oid & atomId)
{
    QMutexLocker lock(&_kernel->_runtimeStateGuard);

    //  Validate parameters
    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();

    //  Do the work
    if (process->_atomInterests.contains(name))
    {   //  Just incremenent the reference count
        Process::_AtomInterest * atomInterest = process->_atomInterests[name];
        atomInterest->_interestCount++;
        if (atomInterest->_interestCount == 0)
        {   //  OOPS! Overflow!
            atomInterest->_interestCount--;
            return KErrno::LimitReached;
        }
        return KErrno::OK;
    }
    else
    {   //  Need to find the Atom...
        Atom * atom;
        if (!_kernel->_atomsByName.contains(name))
        {   //  Atom does not yet exist - create it now
            atom = new Atom(_kernel, name);
            Q_ASSERT(_kernel->_atomsByName.contains(name));
        }
        else
        {
            atom = _kernel->_atomsByName[name];
        }
        //  Create an interest...
        Process::_AtomInterest * atomInterest = new Process::_AtomInterest();
        atomInterest->_process = process;
        atomInterest->_atom = atom;
        atomInterest->_interestCount = 1;
        atomInterest->_process->incrementReferenceCount();  //  we've just stored a reference to Process
        atomInterest->_atom->incrementReferenceCount();     //  we've just stored a reference to Atom
        //  ...record it
        process->_atomInterests.insert(name, atomInterest);
        atom->_atomInterests.insert(atomInterest);
        //  ...and we're done
        atomId = atom->oid();
        return KErrno::OK;
    }
}

KErrno Kernel::SystemCalls::releaseAtom(Thread * thread, Oid atomOid)
{
    QMutexLocker lock(&_kernel->_runtimeStateGuard);

    //  Validate parameters
    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();

    if (!_kernel->_liveObjects.contains(atomOid))
    {
        return KErrno::InvalidParameter;
    }
    Atom * atom = dynamic_cast<Atom*>(_kernel->_liveObjects[atomOid]);
    if (atom == nullptr)
    {
        return KErrno::InvalidParameter;
    }
    QString atomName = atom->name();

    //  If the "process" has no interest in "atom", there's nothing to do
    if (!process->_atomInterests.contains(atomName))
    {
        return KErrno::InvalidParameter;
    }
    //  Decrement interest count...
    Process::_AtomInterest * atomInterest = process->_atomInterests[atomName];
    Q_ASSERT(atomInterest->_interestCount > 0);
    atomInterest->_interestCount--;
    //  ...and drop the interest if the interest count drops to zero
    if (atomInterest->_interestCount == 0)
    {
        atom->_atomInterests.remove(atomInterest);
        process->_atomInterests.remove(atomName);
        process->decrementReferenceCount(); //  we're about to drop a reference to Process from atomInterest
        atom->decrementReferenceCount();    //  we're about to drop a reference to Atom from atomInterest
        delete atomInterest;
    }

    //  TODO can we destroy Atom that no Process has an interest in ?
    //  All done
    return KErrno::OK;
}

KErrno Kernel::SystemCalls::getAtomName(Oid /*atomOid*/, QString & /*name*/)
{
    QMutexLocker lock(&_kernel->_runtimeStateGuard);

    Q_ASSERT(false);    //  TODO implement properly
    return KErrno::NotImplemented;
}

//////////
//  Operations (services and servlets)
KErrno Kernel::SystemCalls::createService(Thread * thread,
                                          const QString & name, unsigned int version,
                                          unsigned int maxParameters, unsigned int backlog,
                                          Handle & handle)
{
    QMutexLocker lock(&_kernel->_runtimeStateGuard);

    //  Validate parameters
    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();

    if (!Kernel::isValidServiceName(name))
    {
        return KErrno::InvalidParameter;
    }
    if (version == 0)
    {
        return KErrno::InvalidParameter;
    }

    //  No duplicates!
    for (Object * obj : _kernel->_liveObjects.values())
    {
        if (Service * srv = dynamic_cast<Service*>(obj))
        {
            if (srv->name() == name && srv->version() == version)
            {   //  OOPS!
                return KErrno::AlreadyExists;
            }
        }
    }

    //  Create a new Service
    Service * service = new Service(_kernel, process, name, version, maxParameters, backlog);
    //  ...and open a handle to it on behalf of the calling Process
    KErrno err = process->openHandle(service, handle);  //  Posts the "HandleOpen" message to the service
    if (err != KErrno::OK)
    {   //  OOPS! The newly created Service cannot be used!
        delete service;
        return err;
    }

    //  Done
    return KErrno::OK;
}

KErrno Kernel::SystemCalls::openService(Thread * thread,
                                        const QString & name, unsigned int version,
                                        Handle & handle)
{
    QMutexLocker lock(&_kernel->_runtimeStateGuard);

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();
}

//////////
//  Operations (miscellaneous)
QVersionNumber Kernel::SystemCalls::getSystemVersion()
{
    QMutexLocker lock(&_kernel->_runtimeStateGuard);
    return QVersionNumber(1, 0, 0);
}

//  End of hadesvm-kernel/Kernel.SystemCalls.cpp
