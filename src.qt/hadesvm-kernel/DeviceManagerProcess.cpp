//
//  hadesvm-kernel/DeviceManagerProcess.cpp
//
//  hadesvm::kernel::DeviceManagerProcess class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Constants
const QString DeviceManagerProcess::Name = "device manager";

//////////
//  DeviceManagerProcess::MainThread
DeviceManagerProcess::MainThread::MainThread(Kernel * kernel, Process * process)
    :   NativeThread(kernel, process)
{
}

DeviceManagerProcess::MainThread::~MainThread()
{
}

Thread::ExitCode DeviceManagerProcess::MainThread::run()
{
    qDebug() << "Starting DeviceManagerMainThread";

    //  Create relevant atoms TODO examining error codes
    systemCalls.getAtom(SystemInterfaces::IUnknown::Name, _interfaceAtomOid_IUnknown);
    systemCalls.getAtom(SystemInterfaces::IUnknown::QueryInterface, _serviceAtomOid_IUnknown_QueryInterface);
    systemCalls.getAtom(SystemInterfaces::IUnknown::HandleOpen, _serviceAtomOid_IUnknown_HandleOpen);
    systemCalls.getAtom(SystemInterfaces::IUnknown::HandleClosed, _serviceAtomOid_IUnknown_HandleClosed);

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
            systemCalls.completeMessage(messageOid, KErrno::OK, QList<Message::Parameter>());
        }
    }

    qDebug() << "Exiting DeviceManagerMainThread";
    systemCalls.closeHandle(_serviceHandle);
    return ExitCode::Success;
}

//  End of hadesvm-kernel/DeviceManagerProcess.cpp
