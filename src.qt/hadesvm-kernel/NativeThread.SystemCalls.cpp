//
//  hadesvm-kernel/NativeThread.SystemCalls.cpp
//
//  hadesvm::kernel::NativeThread::SystemCalls class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

#define HANDLE_TERMINATION_REQUEST()                        \
    if (_nativeThread->_terminationRequested.load())        \
    {                                                       \
        throw Thread::ExitCode::Unknown;                    \
    }

//////////
//  Construction/destruction
NativeThread::SystemCalls::SystemCalls(NativeThread * nativeThread)
    :   _nativeThread(nativeThread)
{
    Q_ASSERT(_nativeThread != nullptr);
}

NativeThread::SystemCalls::~SystemCalls()
{
}

//////////
//  Operations (atoms)
KErrno NativeThread::SystemCalls::getAtom(const QString & name, Oid & atomOid)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.getAtom(_nativeThread, name, atomOid);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::releaseAtom(Oid atomOid)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.releaseAtom(_nativeThread, atomOid);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::getAtomName(Oid atomOid, QString & name)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.getAtomName(atomOid, name);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

//////////
//  Operations (services and servlets)
KErrno NativeThread::SystemCalls::createService(const QString & name, unsigned int version,
                                                unsigned int maxParameters, unsigned int backlog,
                                                Handle & handle)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.createService(_nativeThread, name, version, maxParameters, backlog, handle);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

KErrno NativeThread::SystemCalls::openService(const QString & name, unsigned int version,
                                              Handle & handle)
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.openService(_nativeThread, name, version, handle);
    HANDLE_TERMINATION_REQUEST();
    return result;
}

//////////
//  Operations (miscellaneous)
QVersionNumber NativeThread::SystemCalls::getSystemVersion()
{
    HANDLE_TERMINATION_REQUEST();
    auto result = _nativeThread->_kernel->systemCalls.getSystemVersion();
    HANDLE_TERMINATION_REQUEST();
    return result;
}

//  End of hadesvm-kernel/NativeThread.SystemCalls.cpp
