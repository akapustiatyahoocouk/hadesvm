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
    :   NativeThread(kernel, process)
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
    for (; ; )
    {
        systemCalls.getSystemVersion();
    }
    //  TODO implement
    qDebug() << "Exiting DeviceManagerMainThread";
    return ExitCode::Success;
}

//  End of hadesvm-kernel/DeviceManagerMainThread.cpp
