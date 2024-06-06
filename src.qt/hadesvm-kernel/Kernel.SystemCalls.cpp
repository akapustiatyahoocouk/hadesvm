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

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();

    return process->getAtom(name, atomId);
}

KErrno Kernel::SystemCalls::releaseAtom(Thread * thread, Oid atomOid)
{
    QMutexLocker lock(&_kernel->_runtimeStateGuard);

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();

    return process->releaseAtom(atomOid);
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

    //  Create a new Service...
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

    //  Locate the Service...
    Service * service = nullptr;
    for (Object * obj : _kernel->_liveObjects.values())
    {
        if (Service * s = dynamic_cast<Service*>(obj))
        {   //  Match the name/version
            if (s->name() == name)
            {
                if ((version != 0 && version == s->version()) ||
                    (version == 0 && (service == nullptr || s->version() > service->version())))
                {
                    service = s;
                    break;
                }
            }
        }
    }
    if (service == nullptr)
    {
        return KErrno::DoesNotExist;
    }

    //  ...and open a handle to it on behalf of the calling Process
    KErrno err = process->openHandle(service, handle);  //  Posts the "HandleOpen" message to the service
    if (err != KErrno::OK)
    {   //  OOPS!
        return err;
    }

    //  Done
    return KErrno::OK;
}

//////////
//  Operations (miscellaneous)
QVersionNumber Kernel::SystemCalls::getSystemVersion()
{
    QMutexLocker lock(&_kernel->_runtimeStateGuard);
    return QVersionNumber(1, 0, 0);
}

KErrno Kernel::SystemCalls::closeHandle(Thread * thread, Handle & handle)
{
    QMutexLocker lock(&_kernel->_runtimeStateGuard);

    if (thread == nullptr || thread->kernel() != _kernel || !thread->live())
    {
        return KErrno::InvalidParameter;
    }
    Process * process = thread->process();

    return process->closeHandle(handle);
}

//  End of hadesvm-kernel/Kernel.SystemCalls.cpp
