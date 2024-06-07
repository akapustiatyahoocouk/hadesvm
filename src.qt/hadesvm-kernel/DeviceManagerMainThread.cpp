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
    {   //  TODO implement
        Oid messageOid;
        Oid senderProcessOid;
        Handle senderProcessHandle;
        Oid messageTypeAtomOid;
        QList<Message::Parameter> params;
        KErrno err = systemCalls.getMessage(_serviceHandle, InfiniteTimeout,
                                            messageOid, senderProcessOid, senderProcessHandle,
                                            messageTypeAtomOid, params);
        if (err == KErrno::OK)
        {
            qDebug() << static_cast<uint32_t>(messageOid) << ": " << static_cast<uint32_t>(messageTypeAtomOid);
        }
    }

    qDebug() << "Exiting DeviceManagerMainThread";
    systemCalls.closeHandle(_serviceHandle);
    return ExitCode::Success;
}

//  End of hadesvm-kernel/DeviceManagerMainThread.cpp
