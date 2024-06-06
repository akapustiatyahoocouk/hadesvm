//
//  hadesvm-kernel/DeviceManagerMainThread.cpp
//
//  hadesvm::kernel::DeviceManagerMainThread class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
DeviceManagerMainThread::DeviceManagerMainThread(Kernel * kernel, Process * process)
    :   NativeThread(kernel, process),
        //  Atoms
        _serviceNameAtomOid(Oid::Invalid),
        //  Handles
        _serviceHandle(Handle::Invalid)
{
}

DeviceManagerMainThread::~DeviceManagerMainThread()
{
}

//////////
//  NativeThread
Thread::ExitCode DeviceManagerMainThread::run()
{
    qDebug() << "Starting DeviceManagerMainThread";

    //  Create relevant atoms TODO examining error codes
    systemCalls.getAtom(SystemServices::DeviceManager::Name, _serviceNameAtomOid);

    //  Create services TODO examining error codes
    systemCalls.createService(SystemServices::DeviceManager::Name,
                              SystemServices::DeviceManager::Version,
                              1,
                              0,
                              _serviceHandle);
    for (; ; )
    {
        systemCalls.getSystemVersion();
    }
    //  TODO implement
    qDebug() << "Exiting DeviceManagerMainThread";
    return ExitCode::Success;
}

//  End of hadesvm-kernel/DeviceManagerMainThread.cpp
